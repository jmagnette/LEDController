#include "LEDConsoleOut.h"

static unsigned short rowLength = 0;
static unsigned short columnLength = 0;
static unsigned short numberOfLeds = 0;

static struct RGB* outputGrid = NULL;
static struct pico_socket *ledSocket = NULL;
static struct pico_ip4 myIpv4Addr = {0};
static struct pico_ip4 netMask = {0};
static unsigned char macAddr[6] = {
        0x02, 0x00, 0x11, 0x22, 0x33, 0x44
    };
static struct pico_device *vde_device = NULL;

static char readBuffer[MAX_READ_SIZE];
static char writeBuffer[MAX_WRITE_SIZE];
static uint8_t bytesToWrite = 0;

static uint16_t port;
static char isSocketClosed = 0;


// Function used to set the local size of the 'simulated LED strip' (used for display)
static void setSimulatedSize(unsigned short row, unsigned short column);

// Function that will handle the networking part
static int startNetworkRoutine();
static void ledCommunication(uint16_t event, struct pico_socket *sender);
static int processSocketReading(struct pico_socket *sender);

// Inner functions used to init the local RGBList
static void initRGBList(struct RGB* lineToInit);
static void initRGBListTo(struct RGB* lineToInit, struct RGB RGBValue);

int main()
{
	atexit(&outputCleanUp);

	rowLength = DEFAULT_ROW_NUMBER;
	columnLength = DEFAULT_COLUMN_NUMBER;

	printf("Before we go further, please enter the size of the LED grid you are going to use in the form 'a b':\n");
	printf("(grid size formatted as (a x b), a = 1 for a line. Default is %d x %d, enter nothing to keep default)\n", DEFAULT_ROW_NUMBER, DEFAULT_COLUMN_NUMBER);
	char userInput[100];

	short tmpRows = 0, tmpColumns = 0;
	while (0 >= tmpRows || 0 >= tmpColumns)
	{
		fgets(userInput, sizeof(userInput), stdin);
		if( 0 == strcmp("\n",userInput))
			break;
		else
		{
			sscanf(userInput, "%hu %hu", &tmpRows, &tmpColumns);
			if (0 >= tmpRows || 0 >= tmpColumns)
				printf("Please enter correct value for a and b : ");
			else
			{
				rowLength = tmpRows;
				columnLength = tmpColumns;
			}
		}
	}

	setSimulatedSize(rowLength, columnLength);
	printLedGrid();

	return startNetworkRoutine();
}

int startNetworkRoutine()
{
	int ret;
	pico_string_to_ipv4("10.42.0.10", &myIpv4Addr.addr);
	pico_string_to_ipv4("255.255.255.0", &netMask.addr);
	port  = short_be(7777);

	pico_stack_init();
	vde_device = pico_vde_create("/tmp/led0.ctl","led0", macAddr);

	if (!vde_device)
	{
		fprintf(stderr, "Not capable of linking to vde led0, check if created correctly.\n");
		exit(COM_DEVICE_CANNOT_CREATE);
	}

	pico_ipv4_link_add(vde_device, myIpv4Addr, netMask);

	ledSocket = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP, &ledCommunication);
	if (!ledSocket)
		exit(COM_SOCKET_CANNOT_OPEN);
	printf("Socket opened\n");
	isSocketClosed = 0;

	int yes = 1;
	pico_socket_setoption(ledSocket, PICO_TCP_NODELAY, &yes);

	ret = pico_socket_bind(ledSocket, &myIpv4Addr, &port);
	if (0 != ret)
	{
		ret = pico_err;
		exit(COM_SOCKET_CANNOT_BIND);
	}
	printf("Socket bound\n");

	ret = pico_socket_listen(ledSocket, 1);
	if (0 != ret)
		exit(COM_SOCKET_CANNOT_LISTEN);
	printf("Socket listened\n");

	while(0 == isSocketClosed)
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
			exit(5);
	}

	if (event & PICO_SOCK_EV_WR)
	{
		if (0 < bytesToWrite)
		{
			w = pico_socket_write(sender, &writeBuffer, bytesToWrite);
			if (w < 0)
				exit(5);
			strcpy(writeBuffer, ""); //emptying buffer
			bytesToWrite = 0;
		}
	}

	if (event & PICO_SOCK_EV_CONN)
	{
		/* accept new connection request */
		accepted_socket = pico_socket_accept(sender, &origin, &locPort);
		int yes = 1;
		pico_socket_setoption(accepted_socket, PICO_TCP_NODELAY, &yes);
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
		// when peer closes the socket, it is time to update the LEDs
		printLedGrid();
		pico_socket_close(sender);
	}
}

static int processSocketReading(struct pico_socket *sender)
{
	int bytesRead = 0;
	bytesRead = pico_socket_read(sender, &readBuffer, 4); //Reading message type first

	if (0 > bytesRead)
		return bytesRead;

	readBuffer[4] = '\0';

	uint32_t command = ((uint32_t)readBuffer[0] << 24) + ((uint32_t)readBuffer[1] << 16)
			+ ((uint32_t)readBuffer[2] << 8) + (uint32_t)readBuffer[3];

	struct RGB rgbValue;
	uint16_t rgbAddress = 0;

	switch (command)
	{
		case LED_SIZE:
			//Nothing else to read than the SIZE command it self
			strcpy(writeBuffer, "SIZE");
			writeBuffer[4] = rowLength >> 8;
			writeBuffer[5] = rowLength;

			writeBuffer[6] = columnLength >> 8;
			writeBuffer[7] = columnLength;

			bytesToWrite = 8;

			break;

		case LED_RESET:
			bytesRead = pico_socket_read(sender, &readBuffer, 3); //Reading RGB value
			if (0 > bytesRead)
				return bytesRead;

			rgbValue.red = readBuffer[0];
			rgbValue.green = readBuffer[1];
			rgbValue.blue = readBuffer[2];

			initRGBListTo(outputGrid,rgbValue);

			strcpy(writeBuffer, "RSET");
			bytesToWrite = 4;
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

			strcpy(writeBuffer, "SLED");
			bytesToWrite = 4;
			break;

		default:
			//unknown message type
			fprintf(stderr,"Unknown message type received from output socket\n");
			return 1;
	}

	return 0;
}

void outputCleanUp()
{
	printf("Cleaning before closing...\n");
	fflush(stdout);

	if (ledSocket)
		pico_socket_close(ledSocket);

	free(outputGrid);
	outputGrid = NULL;
	isSocketClosed = 1;
}

void setSimulatedSize(unsigned short row, unsigned short column)
{
	numberOfLeds = row * column;
	if (NULL != outputGrid)
	{
		free(outputGrid);
	}
	outputGrid = malloc(numberOfLeds * sizeof(*outputGrid) );
	initRGBList(outputGrid);
} 

void setLed(unsigned short address, struct RGB RGBValue)
{
	if (address < (rowLength * columnLength))
	{
		outputGrid[address].red = RGBValue.red;
		outputGrid[address].green = RGBValue.green;
		outputGrid[address].blue = RGBValue.blue;
	}
}

// inner function used to print one 'simulated LED' to the screen
void printLed(Byte red, Byte green, Byte blue)
{
	printf("|\x1b[48;2;%u;%u;%um_\x1b[0m|  ",red,green,blue);
}

void printLedGrid()
{
	system("clear");
	for(int index = 0; index < numberOfLeds; ++index)
	{
		printLed(outputGrid[index].red, outputGrid[index].green, outputGrid[index].blue);
		if (0 == (index+1)%columnLength )
		{
			printf("\n");
		}
	}
	printf("\n");
	fflush(stdout);
}

void initRGBList(struct RGB* lineToInit)
{
	initRGBListTo(lineToInit, (struct RGB){0,0,0});
}

void initRGBListTo(struct RGB* lineToInit, struct RGB RGBValue)
{
	for(int index = 0; index < numberOfLeds; ++index)
	{
		lineToInit[index].red = RGBValue.red;
		lineToInit[index].green = RGBValue.green;
		lineToInit[index].blue = RGBValue.blue;
	}
}





