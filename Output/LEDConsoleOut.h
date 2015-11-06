#ifndef LEDCONSOLEOUT_H
#define LEDCONSOLEOUT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../MainLib/LedRGB.h"

void outputCleanUp();
void setMaxLineLength(unsigned short length);

void setLed(unsigned short address, struct RGB RGBValue);
void printLedGrid();

#endif
