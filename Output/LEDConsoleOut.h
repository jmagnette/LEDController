#ifndef LEDCONSOLEOUT_H
#define LEDCONSOLEOUT_H

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../MainLib/Byte.h"
#include "../MainLib/LedRGB.h"


void setMaxLineLength(unsigned short length);
unsigned short getMaxLineLength();

void printLed(Byte red, Byte green, Byte blue);
void printLine(struct RGB* lineToPrint);
void printOneColourLine(struct RGB RGBValue);
void initRGBList(struct RGB* lineToInit);
void initRGBListTo(struct RGB* lineToInit, struct RGB RGBValue);

void printBlinkingExample();
void printCyclingExample();


#endif
