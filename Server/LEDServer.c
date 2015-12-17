#include "LEDServer.h"

static unsigned short numberOfRows = 0, numberOfColumns = 0;
static struct pico_socket *outputSocket = NULL;
static struct pico_device *vde_device = NULL;
static struct pico_ip4 destIpv4Addr = {0};
static struct pico_ip4 ipv4Addr = {0};
static struct pico_ip4 netMask = {0};
static uint16_t port = 7777;

static unsigned char macAddr[6] = {
        0x02, 0x00, 0xaa, 0xbb, 0xcc, 0xdd
    };

static unsigned char responseFromOutput = 0;
static unsigned char isCommunicationInitialized = 0;
static unsigned char isSocketOpened = 0;

static char readBuffer[MAX_READ_SIZE];
static char writeBuffer[MAX_WRITE_SIZE];
static uint8_t bytesToWrite = 0;

static void connectToOutput();
static void initCommunication();
static void outputCommunication(uint16_t event, struct pico_socket *sender);
static int processSocketReading(struct pico_socket *sender);

void serverClose()
{
	pico_socket_close(outputSocket);
	while (0 != isSocketOpened)
	{
		pico_stack_tick();
		nanosleep(&(struct timespec){0,5*1000000},NULL);
	}
}

int getOutputInformations()
{
	char userInput[100];
	char userIpAddress[16] = "";
	uint16_t userPort = 0;

	printf("Please enter the ipv4 output address to connect to as well as the port that will be used :\n");
	printf("(entry of the form xxx.xxx.xxx.xxx 12345, default value : '10.42.0.10:7777', enter nothing to keep default)\n");

	uint32_t tempAddr;
	while (0 != pico_string_to_ipv4(userIpAddress, &tempAddr) || 0 == userPort)
	{
		fgets(userInput, sizeof(userInput), stdin);
		if( 0 == strcmp("\n",userInput))
		{
			strcpy(userIpAddress, "10.42.0.10");
			userPort = 7777;
		}
		else
		{
			sscanf(userInput, "%s %hu", userIpAddress, &userPort);
			if (0 != pico_string_to_ipv4(userIpAddress, &tempAddr) || 0 == userPort)
			{
				printf("Please enter correct value : ");
			}
		}
	}

	port = short_be(userPort);
	destIpv4Addr.addr = tempAddr;

	responseFromOutput = 0;
	strcpy(writeBuffer, "SIZE");
	bytesToWrite = 4;
	connectToOutput();

	while (0 == responseFromOutput)
	{
		pico_stack_tick();
		nanosleep(&(struct timespec){0,5*1000000},NULL);
	}

	printf("Output size is %d x %d\n", numberOfRows, numberOfColumns);

	return 0;
}

void translateToOutput(struct AddressableLED** sequence, unsigned short sequenceLength)
{
	if (0 == isSocketOpened)
		connectToOutput();

	uint16_t address = 0;
	for (int index = 0; index < sequenceLength; index ++)
	{
		// special address for full line change
		if (USHRT_MAX == sequence[index]->address.row && USHRT_MAX == sequence[index]->address.column)
		{
			strcpy(writeBuffer, "RSET");

			writeBuffer[4] = sequence[index]->ledValue.red;
			writeBuffer[5] = sequence[index]->ledValue.green;
			writeBuffer[6] = sequence[index]->ledValue.blue;

			bytesToWrite = 7;
		}
		else if (sequence[index]->address.row < numberOfRows && sequence[index]->address.column < numberOfColumns)
		{
			strcpy(writeBuffer, "SLED");
			address = (sequence[index]->address.row * numberOfColumns) + sequence[index]->address.column;
			writeBuffer[4] = address >> 8;
			writeBuffer[5] = address;

			writeBuffer[6] = sequence[index]->ledValue.red;
			writeBuffer[7] = sequence[index]->ledValue.green;
			writeBuffer[8] = sequence[index]->ledValue.blue;

			bytesToWrite = 9;
		}

		while (0 < bytesToWrite)
		{
			pico_stack_tick();
			nanosleep(&(struct timespec){0,5*1000000},NULL);
		}
	} 

	strcpy(writeBuffer, "UPDT");
	bytesToWrite = 4;
	while (0 < bytesToWrite)
	{
		pico_stack_tick();
		nanosleep(&(struct timespec){0,5*1000000},NULL);
	}

}

unsigned short getRowSize()
{
	return numberOfRows;
}
unsigned short getColumnSize()
{
	return numberOfColumns;
}

void connectToOutput()
{
	int ret;
	if (0 == isCommunicationInitialized)
		initCommunication();

	outputSocket = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP, &outputCommunication);
	if (!outputSocket)
		exit(COM_SOCKET_CANNOT_OPEN);

	ret = pico_socket_connect(outputSocket, &destIpv4Addr, port);
	if (0 != ret)
	{
		ret = pico_err;
		exit(COM_SOCKET_CANNOT_CONNECT);
	}

	while (0 == isSocketOpened)
	{
		pico_stack_tick();
		nanosleep(&(struct timespec){0,5*1000000},NULL);
	}
}

void initCommunication()
{
	pico_string_to_ipv4("10.42.0.5", &ipv4Addr.addr);
	pico_string_to_ipv4("255.255.255.0", &netMask.addr);

	pico_stack_init();
	vde_device = pico_vde_create("/tmp/led0.ctl","led0", macAddr);

	if (!vde_device)
	{
		fprintf(stderr, "Not capable of linking to vde led0, check if created correctly.\n");
		exit(COM_DEVICE_CANNOT_CREATE);
	}

	pico_ipv4_link_add(vde_device, ipv4Addr, netMask);
	isCommunicationInitialized = 1;
}

void outputCommunication(uint16_t event, struct pico_socket *sender)
{
	int w;

	if (event & PICO_SOCK_EV_RD)
	{
		if ( 0 > processSocketReading(sender))
		{
			printf("Socket Error received when reading: %s. Bailing out.\n", strerror(pico_err));
			exit(COM_SOCKET_ERROR_READ);
		}
	}

	if (event & PICO_SOCK_EV_CONN) {
		isSocketOpened = 1;
		char ipAddress[16];
		pico_ipv4_to_string(ipAddress, sender->remote_addr.ip4.addr);
		printf("Connection established with server. address = %s:%d\n", ipAddress , short_be(sender->remote_port));
	}

	if (event & PICO_SOCK_EV_FIN) {
		isSocketOpened = 0;
		char ipAddress[16];
		pico_ipv4_to_string(ipAddress, sender->remote_addr.ip4.addr);
	    printf("Socket closed. Exit normally. address = %s:%d\n", ipAddress , short_be(sender->remote_port));
	}

	if (event & PICO_SOCK_EV_ERR) {
	    printf("Socket error received: %s. Bailing out.\n", strerror(pico_err));
	    exit(COM_SOCKET_ERROR);
	}

	if (event & PICO_SOCK_EV_CLOSE) {
	   printf("Socket received close from peer - Wrong case if not all client data sent!\n");
	   pico_socket_close(sender);
	}

	if (event & PICO_SOCK_EV_WR) {
		if (0 < bytesToWrite)
		{
			w = pico_socket_write(sender, &writeBuffer, bytesToWrite);
			if (w < 0)
			{
				printf("Socket error received when writing: %s. Bailing out.\n", strerror(pico_err));
				exit(COM_SOCKET_ERROR_WRITE);
			}
			strcpy(writeBuffer, ""); //emptying buffer
			bytesToWrite = 0;
		}
	}
}

static int processSocketReading(struct pico_socket *sender)
{
	int bytesRead = 0;
	bytesRead = pico_socket_read(sender, &readBuffer, 4); //Reading message type first

	if (0 >= bytesRead)
		return bytesRead;

	uint32_t command = ((uint32_t)readBuffer[0] << 24) + ((uint32_t)readBuffer[1] << 16)
			+ ((uint32_t)readBuffer[2] << 8) + (uint32_t)readBuffer[3];

	switch (command)
	{
		case LED_SIZE:
			bytesRead = pico_socket_read(sender, &readBuffer, 4); //Reading row and column size
			if (0 > bytesRead)
				return bytesRead;

			numberOfRows = ((uint16_t)readBuffer[0] << 8) + (uint16_t)readBuffer[1];
			numberOfColumns = ((uint16_t)readBuffer[2] << 8) + (uint16_t)readBuffer[3];

			//pico_socket_close(sender);
			responseFromOutput = 1;

			break;

		case LED_RESET:
		case LED_SET:
			//Nothing to do on this side (just an ack from output)
			break;

		default:
			//unknown message type
			fprintf(stderr,"Unknown message type received from output socket\n");
			return 1;
	}

	return 0;
}


