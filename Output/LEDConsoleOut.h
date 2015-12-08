#ifndef LEDCONSOLEOUT_H
#define LEDCONSOLEOUT_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../MainLib/include/LedRGB.h"

#include "../MainLib/include/pico_stack.h"
#include "../MainLib/include/pico_socket.h"
#include "../MainLib/include/pico_ipv4.h"

#define DEFAULT_ROW_NUMBER  3
#define DEFAULT_COLUMN_NUMBER  10

enum E_ConsoleError
{
	CSL_SUCCESS = 0,
	CSL_SOCKET_CANNOT_OPEN,
	CSL_SOCKET_CANNOT_BIND,
	CSL_SOCKET_CANNOT_LISTEN,
	CSL_SOCKET_ERROR
};

/*
	Function that will handle all the cleanup task to do on the output side (free memory)
*/
void outputCleanUp();

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
