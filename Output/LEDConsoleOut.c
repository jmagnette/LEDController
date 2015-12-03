#include "LEDConsoleOut.h"

static unsigned short rowLength = 0;
static unsigned short columnLength = 0;
static unsigned short numberOfLeds = 0;

static struct RGB* outputGrid = NULL;

// Inner functions used to init the local RGBList
void initRGBList(struct RGB* lineToInit);
void initRGBListTo(struct RGB* lineToInit, struct RGB RGBValue);

void outputCleanUp()
{
	free(outputGrid);
	outputGrid = NULL;
}

void setSimulatedSize(unsigned short row, unsigned short column)
{
	rowLength = row;
	columnLength = column;
	numberOfLeds = rowLength * columnLength;
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





