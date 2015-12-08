#include "LEDConsoleOut.h"

static unsigned short rowLength = 0;
static unsigned short columnLength = 0;
static unsigned short numberOfLeds = 0;

static struct RGB* outputGrid = NULL;
static struct pico_socket *ledSocket = NULL;
static struct pico_ip4 ipv4Addr = {};
static uint16_t port = 7777;
static char isSocketClosed = 0;


// Function used to set the local size of the 'simulated LED strip' (used for display)
static void setSimulatedSize(unsigned short row, unsigned short column);

// Function that will handle the networking part
static int startNetworkRoutine();
static void ledCommunication(uint16_t event, struct pico_socket *sender);

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

	return startNetworkRoutine();
}

int startNetworkRoutine()
{
	int ret;
	//pico_string_to_ipv4("127.0.0.1", &ipv4Addr.addr);
	pico_stack_init();
	ledSocket = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP, &ledCommunication);
	if (!ledSocket)
		exit(CSL_SOCKET_CANNOT_OPEN);
	printf("Socket opened\n");
	isSocketClosed = 0;

	ret = pico_socket_bind(ledSocket, &ipv4Addr, &port);
	if (0 != ret)
		exit(CSL_SOCKET_CANNOT_BIND);
	printf("Socket bound\n");

	ret = pico_socket_listen(ledSocket, 1);
	if (0 != ret)
		exit(CSL_SOCKET_CANNOT_LISTEN);
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
	uint16_t port;
	char peer[30];

	if (event & PICO_SOCK_EV_RD)
	{

	}

	if (event & PICO_SOCK_EV_CONN)
	{
		/* accept new connection request */
		accepted_socket = pico_socket_accept(ledSocket, &origin, &port);
		/* convert peer IP to string */
		pico_ipv4_to_string(peer, origin.addr);
		/* print info */
		printf("Connection established with %s:%d.\n", peer, short_be(port));
	}

	if (event & PICO_SOCK_EV_FIN) {
		printf("Socket closed. Exit normally. \n");
		isSocketClosed = 1;
	}

	if (event & PICO_SOCK_EV_ERR) {
		printf("Socket Error received: %s. Bailing out.\n", strerror(pico_err));
		exit(CSL_SOCKET_ERROR);
	}

	if (event & PICO_SOCK_EV_CLOSE) {
		printf("Socket received close from peer.\n");
		/* shutdown write side of socket */
		pico_socket_shutdown(ledSocket, PICO_SHUT_WR);
	}
}

void outputCleanUp()
{
	printf("Cleaning before closing...\n");
	fflush(stdout);
	if (ledSocket)
		pico_socket_close(ledSocket);

	free(outputGrid);
	outputGrid = NULL;
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





