#ifndef LEDCONSOLEOUT_H
#define LEDCONSOLEOUT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../MainLib/include/LedRGB.h"

/*
	Function that will handle all the cleanup task to do on the output side (free memory)
*/
void outputCleanUp();

/*
	Function used to set the local size of the 'simulated LED strip' (used for display)
*/
void setSimulatedSize(unsigned short row, unsigned short column);

/*
	Function that will change the value of a local Led, given its address and its RGB value.
	If address is out of range, this won't do anything.
	(call printLedGrid to show the change)
*/
void setLed(unsigned short address, struct RGB RGBValue);

/*
	Calling this function will print the 'simulated LED strip' to the screen
*/
void printLedGrid();

#endif
