#include "LEDConsoleOut.h"

unsigned short maxLineLength = 0;
struct RGB* outputGrid = NULL;

void initRGBList(struct RGB* lineToInit);
void initRGBListTo(struct RGB* lineToInit, struct RGB RGBValue);

void outputCleanUp()
{
	if (NULL != outputGrid)
	{
		free(outputGrid);
		outputGrid = NULL;
	}
}

void setMaxLineLength(unsigned short length)
{
	maxLineLength = length;
	if (NULL != outputGrid)
	{
		free(outputGrid);
	}
	outputGrid = malloc(maxLineLength * sizeof(*outputGrid) );
	initRGBList(outputGrid);
} 

void setLed(unsigned short address, struct RGB RGBValue)
{
	if (address < maxLineLength)
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
	for(int index = 0; index < maxLineLength; ++index)
	{
		printLed(outputGrid[index].red, outputGrid[index].green, outputGrid[index].blue);
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
	for(int index = 0; index < maxLineLength; ++index)
	{
		lineToInit[index].red = RGBValue.red;
		lineToInit[index].green = RGBValue.green;
		lineToInit[index].blue = RGBValue.blue;
	}
}





