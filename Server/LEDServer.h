#ifndef LEDSERVER_H
#define LEDSERVER_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <string.h>

#include <time.h>
#include "../MainLib/include/LedRGB.h"

/*
	Function used to close the server properly
*/
void serverClose();

/*
	Function used to take a sequence of LED value and translate it for output (+ sending it)
*/
void translateToOutput(struct AddressableLED** sequence, unsigned short sequenceLength);

unsigned short getRowSize();
unsigned short getColumnSize();

#endif
