#include "LEDConsoleOut.h"

#define NUMBER_OF_LED  300

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
static uint32_t bufferSize = 0;

static void init_timer_dma();



int setupNetwork(struct pico_ip4* obtainedIpv4)
{
	// TO DO
	return 0;
}

void initLedGrid()
{
	// the buffersize is calculated by multiplying the number of LEDs we have by the actual size needed in memory to store the value for the DMA (24 bytes per LED)
	// at which we need to add the extra down time specified in the ws2812b datasheet (at least 50 µs)
	// This down time will be extra bytes added at the end of the array
	// We obtain the number by dividing 50000ns by 1250ns (see datasheet) and adding 2 more to be sure to wait at least 50µs

	uint8_t extraWait = 50000 / 1250 + 2;
	bufferSize = NUMBER_OF_LED * sizeof(struct WS2812bLED) + extraWait * sizeof(Byte);
	outputGrid = malloc(bufferSize);

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


/*
 * Decided to go with timer 4_channel 1 and DMA1 so that it uses the stream 0 of the DMA
 * Will use byte transfer with DMA
 */
void init_timer_dma()
{

}
