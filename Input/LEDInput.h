#ifndef LEDINPUT_H
#define LEDINPUT_H

#define MAXLEDINPUT 2
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../MainLib/LedRGB.h"
#include "../Server/LEDServer.h"

enum E_LedInput
{
	EXAMPLES,
	MANUAL
};

int beginConsoleInput();

#endif
