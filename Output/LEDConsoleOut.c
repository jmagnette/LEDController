#include "LEDConsoleOut.h"

static struct pico_socket *ledSocket = NULL;
static struct pico_ip4 myIpv4Addr = {0};
static uint16_t port;

static char readBuffer[MAX_READ_SIZE];
static char writeBuffer[MAX_WRITE_SIZE];
static uint8_t bytesToWrite = 0;

static char isSocketOpened = 0;


// Function that will handle the networking part
static int startNetworkRoutine();
static void ledCommunication(uint16_t event, struct pico_socket *sender);
static int processSocketReading(struct pico_socket *sender);

static void outputCleanUp();

// Function used to set all the LEDs to one RGB value
static void initRGBListTo(struct RGB RGBValue);



int main()
{
	atexit(&outputCleanUp);

	initLedGrid();

	printLedGrid();

	return startNetworkRoutine();
}

int startNetworkRoutine()
{
	int ret;
	pico_stack_init();
	port  = short_be(7777);

	ret = setupNetwork(&myIpv4Addr);
	if (0 != ret)
		exit(ret);

	ledSocket = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP, &ledCommunication);
	if (!ledSocket)
		exit(COM_SOCKET_CANNOT_OPEN);
	printf("Socket opened\n");
	isSocketOpened = 1;

	ret = pico_socket_bind(ledSocket, &myIpv4Addr, &port);
	if (0 != ret)
		exit(COM_SOCKET_CANNOT_BIND);

	printf("Socket bound\n");

	ret = pico_socket_listen(ledSocket, 1);
	if (0 != ret)
		exit(COM_SOCKET_CANNOT_LISTEN);

	printf("Socket listened\n");

	while(1 == isSocketOpened)
	{
		pico_stack_tick();
		nanosleep(&(struct timespec){0,5*1000000},NULL);
	}

	return 0;
}

void ledCommunication(uint16_t event, struct pico_socket *sender)
{
	struct pico_socket *accepted_socket;
	struct pico_ip4 origin;
	uint16_t locPort;
	char peer[30];
	int w;

	if (event & PICO_SOCK_EV_RD)
	{
		if ( 0 > processSocketReading(sender))
		{
			printf("Socket Error received when reading: %s. Bailing out.\n", strerror(pico_err));
			exit(COM_SOCKET_ERROR_READ);
		}
	}

	if (event & PICO_SOCK_EV_WR)
	{
		if (0 < bytesToWrite)
		{
			w = pico_socket_write(sender, &writeBuffer, bytesToWrite);
			if (w < 0)
			{
				printf("Socket Error received when writing: %s. Bailing out.\n", strerror(pico_err));
				exit(COM_SOCKET_ERROR_WRITE);
			}

			strcpy(writeBuffer, ""); //emptying buffer
			bytesToWrite = 0;
		}
	}

	if (event & PICO_SOCK_EV_CONN)
	{
		/* accept new connection request */
		accepted_socket = pico_socket_accept(sender, &origin, &locPort);

		/* convert peer IP to string */
		pico_ipv4_to_string(peer, origin.addr);

		if (!accepted_socket)
		{
			printf("Socket Error received: %s. cannot accept connection from %s:%d.\n", strerror(pico_err), peer, short_be(locPort));
		}
		else
		{
			/* print info */
			printf("Connection established with %s:%d.\n", peer, short_be(locPort));
		}
	}

	if (event & PICO_SOCK_EV_FIN) {
		printf("Socket closed. Exit normally. \n");
	}

	if (event & PICO_SOCK_EV_ERR) {
		printf("Socket Error received: %s. Bailing out.\n", strerror(pico_err));
		exit(COM_SOCKET_ERROR);
	}

	if (event & PICO_SOCK_EV_CLOSE) {
		printf("Socket received close from peer.\n");
		pico_socket_close(sender);
	}
}

static int processSocketReading(struct pico_socket *sender)
{
	int bytesRead = 0;
	bytesRead = pico_socket_read(sender, &readBuffer, 4); //Reading message type first

	if (0 >= bytesRead)
		return bytesRead;

	readBuffer[4] = '\0';

	uint32_t command = ((uint32_t)readBuffer[0] << 24) + ((uint32_t)readBuffer[1] << 16)
			+ ((uint32_t)readBuffer[2] << 8) + (uint32_t)readBuffer[3];

	struct RGB rgbValue;
	uint16_t rgbAddress = 0;

	switch (command)
	{
		case LED_SIZE:
			//Nothing else to read than the SIZE command itself

			//Answers with the actual size
			strcpy(writeBuffer, "SIZE");
			writeBuffer[4] = getRowLength() >> 8;
			writeBuffer[5] = getRowLength();

			writeBuffer[6] = getColumnLength() >> 8;
			writeBuffer[7] = getColumnLength();

			bytesToWrite = 8;

			break;

		case LED_RESET:
			bytesRead = pico_socket_read(sender, &readBuffer, 3); //Reading RGB value
			if (0 > bytesRead)
				return bytesRead;

			rgbValue.red = readBuffer[0];
			rgbValue.green = readBuffer[1];
			rgbValue.blue = readBuffer[2];

			initRGBListTo(rgbValue);
			break;

		case LED_SET:
			bytesRead = pico_socket_read(sender, &readBuffer, 5); //Reading Addr + RGB value
			if (0 > bytesRead)
				return bytesRead;

			rgbAddress = ((uint16_t)readBuffer[0] << 8) + (uint16_t)readBuffer[1];
			rgbValue.red = readBuffer[2];
			rgbValue.green = readBuffer[3];
			rgbValue.blue = readBuffer[4];

			setLed(rgbAddress, rgbValue);
			break;

		case LED_UPDATE:
			printLedGrid();
			break;

		default:
			//unknown message type
			fprintf(stderr,"Unknown message type received from output socket\n");
			return 1;
	}

	return 0;
}

void initRGBListTo(struct RGB RGBValue)
{
	for(int index = 0; index < ( getRowLength() * getColumnLength() ); ++index)
	{
		setLed(index, RGBValue);
	}
}

void outputCleanUp()
{
	cleanLedGrid();

	if (ledSocket)
		pico_socket_close(ledSocket);

	isSocketOpened = 0;
}

