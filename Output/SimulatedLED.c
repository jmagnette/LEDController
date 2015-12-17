#include "LEDConsoleOut.h"

#define DEFAULT_ROW_NUMBER  3
#define DEFAULT_COLUMN_NUMBER  10

static struct pico_ip4 netMask = {0};
static unsigned char macAddr[6] = {
        0x02, 0x00, 0x11, 0x22, 0x33, 0x44
    };
static struct pico_device *vde_device = NULL;

static unsigned short rowLength;
static unsigned short columnLength;
static unsigned short numberOfLeds;

static struct RGB* outputGrid = NULL;

// inner function used to print one 'simulated LED' to the screen
static void printLed(Byte red, Byte green, Byte blue);



int setupNetwork(struct pico_ip4* obtainedIpv4)
{
	pico_string_to_ipv4("10.42.0.10", &obtainedIpv4->addr);
	pico_string_to_ipv4("255.255.255.0", &netMask.addr);

	vde_device = pico_vde_create("/tmp/led0.ctl","led0", macAddr);

	if (!vde_device)
	{
		fprintf(stderr, "Not capable of linking to vde led0, check if created correctly.\n");
		return COM_DEVICE_CANNOT_CREATE;
	}

	pico_ipv4_link_add(vde_device, *obtainedIpv4, netMask);

	return 0;
}

void initLedGrid()
{

	system("clear");
	printf("Before we go further, please enter the size of the LED grid you are going to use in the form 'a b':\n");
	printf("(grid size formatted as (a x b), a = 1 for a line. Default is %d x %d, enter nothing to keep default)\n", DEFAULT_ROW_NUMBER, DEFAULT_COLUMN_NUMBER);
	char userInput[100];

	unsigned short tmpRows = 0, tmpColumns = 0;
	while (0 >= tmpRows || 0 >= tmpColumns)
	{
		fgets(userInput, sizeof(userInput), stdin);
		if( 0 == strcmp("\n",userInput))
		{
			rowLength = DEFAULT_ROW_NUMBER;
			columnLength = DEFAULT_COLUMN_NUMBER;
			break;
		}
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

	numberOfLeds = rowLength * columnLength;
	if (NULL != outputGrid)
	{
		free(outputGrid);
	}
	outputGrid = malloc(numberOfLeds * sizeof(*outputGrid) );

	for(int index = 0; index < numberOfLeds; ++index)
	{
		setLed(index, (struct RGB){0,0,0});
	}

}

unsigned short getRowLength()
{
	return rowLength;
}

unsigned short getColumnLength()
{
	return columnLength;
}

void setLed(unsigned short address, struct RGB RGBValue)
{
	if (address < numberOfLeds)
	{
		outputGrid[address].red = RGBValue.red;
		outputGrid[address].green = RGBValue.green;
		outputGrid[address].blue = RGBValue.blue;
	}
}

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

void cleanLedGrid()
{
	printf("Cleaning before closing...\n");
	fflush(stdout);

	free(outputGrid);
	outputGrid = NULL;
}

