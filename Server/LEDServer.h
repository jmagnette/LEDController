#ifndef LEDSERVER_H
#define LEDSERVER_H

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <string.h>

#include <time.h>
#include "../Output/LEDConsoleOut.h"
#include "../MainLib/LedRGB.h"

/*
	Temporary function used until networking is in place
	(will ask the output size to be able to filter/translate the input)
*/
void askForOutputLength();

/*
	Function used to close the server properly
*/
void serverClose();

/*
	Function used to take a sequence of LED value and translate it for output (+ sending it)
*/
void translateToOutput(struct AddressableLED** sequence, unsigned short sequenceLength);

#endif
