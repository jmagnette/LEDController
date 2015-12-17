#include "LEDConsoleOut.h"

#define NUMBER_OF_LED  300

//

/*
 * Structure that reflects what has to be done to be able to use DMA and PWM to transmit the RGB value to the ws2812b
 * The transmission order of the RGB value for the ws2812b is green then red then blue (transmitting higher bit first)
 */
struct WS2812bLED{
	Byte green_7;
	Byte green_6;
	Byte green_5;
	Byte green_4;
	Byte green_3;
	Byte green_2;
	Byte green_1;
	Byte green_0;

	Byte red_7;
	Byte red_6;
	Byte red_5;
	Byte red_4;
	Byte red_3;
	Byte red_2;
	Byte red_1;
	Byte red_0;

	Byte blue_7;
	Byte blue_6;
	Byte blue_5;
	Byte blue_4;
	Byte blue_3;
	Byte blue_2;
	Byte blue_1;
	Byte blue_0;
};

static struct WS2812bLED* outputGrid = NULL;



int setupNetwork(struct pico_ip4* obtainedIpv4)
{
	// TO DO
	return 0;
}

void initLedGrid()
{
	// TO DO
}

unsigned short getRowLength()
{
	return 1;  // we consider the strip as only one row
}

unsigned short getColumnLength()
{
	return NUMBER_OF_LED;
}

void setLed(unsigned short address, struct RGB RGBValue)
{
	if (address < NUMBER_OF_LED)
	{
		// TO DO
	}
}

void printLedGrid()
{
	// TO DO
}

void cleanLedGrid()
{
	free(outputGrid);
	outputGrid = NULL;
}
