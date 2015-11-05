#include "LEDConsoleOut.h"
unsigned short maxLineLength = 0;

void setMaxLineLength(unsigned short length)
{
	maxLineLength = length;
} 

unsigned short getMaxLineLength()
{
	return maxLineLength;
}


void printLed(Byte red, Byte green, Byte blue)
{
	printf("|\x1b[48;2;%u;%u;%um_\x1b[0m|  ",red,green,blue);
	fflush(stdout);
}

void printLine(struct RGB* lineToPrint)
{
	for(int index = 0; index < getMaxLineLength(); ++index)
	{
		printLed(lineToPrint[index].red, lineToPrint[index].green, lineToPrint[index].blue);
	}
}

void printOneColourLine(struct RGB RGBValue)
{
	for(int index = 0; index < getMaxLineLength(); ++index)
	{
		printLed(RGBValue.red, RGBValue.green, RGBValue.blue);
	}
}

void initRGBList(struct RGB* lineToInit)
{
	initRGBListTo(lineToInit, (struct RGB){0,0,0});
}

void initRGBListTo(struct RGB* lineToInit, struct RGB RGBValue)
{
	for(int index = 0; index < getMaxLineLength(); ++index)
	{
		lineToInit[index].red = RGBValue.red;
		lineToInit[index].green = RGBValue.green;
		lineToInit[index].blue = RGBValue.blue;
	}
}





void printBlinkingExample()
{
	setMaxLineLength(10);
	printf("\n\n");
	struct RGB LEDs[getMaxLineLength()];
	struct RGB secondLEDs[getMaxLineLength()];
	
	struct AddressableLED input[] =
	{
		{ 3, {255,100,0} }
	};
	
	initRGBList(LEDs);
	initRGBList(secondLEDs);
	
	for(unsigned int index = 0; index < (sizeof(input)/sizeof(input[0])) ; ++index)
	{
		LEDs[input[index].address] = input[index].ledValue;
		secondLEDs[input[index].address] = input[index].ledValue;
	}
	
	secondLEDs[4] = (struct RGB){ .red=165, .green=242, .blue=243};
	
	for(int index = 0; index < 10; index ++)
	{
		printf("\r %d a :", index);
		printLine(LEDs);
		nanosleep(&(struct timespec){0,500*1000000},NULL);
		printf("\r %d b :", index);
		printLine(secondLEDs);
		nanosleep(&(struct timespec){0,500*1000000},NULL);
	}
	
	printf("\n");
	printf("\n");
}

void printCyclingExample()
{
	setMaxLineLength(10);
	printf("\n\n");
	struct RGB LEDs[getMaxLineLength()];
	initRGBList(LEDs);
	struct RGB initialRGB = {LEDs[0].red, LEDs[0].green, LEDs[0].blue};
	struct RGB loadingRGB = { 165, 242, 243};
	
	for(int loopNumber = 0; loopNumber < 10; ++loopNumber)
	{
		for(unsigned short index = 0; index < getMaxLineLength(); ++index)
		{
			printf("\r");
			if (0 == index)
			{
				LEDs[getMaxLineLength() - 1].red = initialRGB.red;
				LEDs[getMaxLineLength() - 1].green = initialRGB.green;
				LEDs[getMaxLineLength() - 1].blue = initialRGB.blue;
			}
			else
			{
				LEDs[index - 1].red = initialRGB.red;
				LEDs[index - 1].green = initialRGB.green;
				LEDs[index - 1].blue = initialRGB.blue;
			}
		
			LEDs[index].red = loadingRGB.red;
			LEDs[index].green = loadingRGB.green;
			LEDs[index].blue = loadingRGB.blue;
			printLine(LEDs);
			nanosleep(&(struct timespec){0,100*1000000},NULL);
		}
		printf("\r");
		LEDs[getMaxLineLength() - 1].red = initialRGB.red;
		LEDs[getMaxLineLength() - 1].green = initialRGB.green;
		LEDs[getMaxLineLength() - 1].blue = initialRGB.blue;
		printLine(LEDs);
	}
	
	printf("\n");
	printf("\n");
	
}





