#ifndef LEDINPUT_H
#define LEDINPUT_H

#define MAXLEDINPUT 4
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../MainLib/LedRGB.h"
#include "../Server/LEDServer.h"
#include "../Server/JSONToLeds.h"

enum E_LedInput
{
	LED_EXAMPLES,
	LED_MANUAL,
	LED_FILE,
	LED_NETWORK
};

int beginConsoleInput();

#endif
