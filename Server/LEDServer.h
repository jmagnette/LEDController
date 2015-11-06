#ifndef LEDSERVER_H
#define LEDSERVER_H

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <string.h>

#include <time.h>
#include "../Output/LEDConsoleOut.h"
#include "../MainLib/LedRGB.h"

void askForOutputLength();
void serverClose();

void translateToOutput(struct AddressableLED* sequence, unsigned short sequenceLength);

#endif
