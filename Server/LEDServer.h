#ifndef LEDSERVER_H
#define LEDSERVER_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#define MAX_READ_SIZE 10
#define MAX_WRITE_SIZE 10

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <time.h>
#include "../MainLib/include/LedRGB.h"

#include "../MainLib/include/pico_stack.h"
#include "../MainLib/include/pico_dev_vde.h"
#include "../MainLib/include/pico_ipv4.h"
#include "../MainLib/include/pico_socket.h"

/*
	Function used to close the server properly
*/
void serverClose();

/*
	Function used to take a sequence of LED value and translate it for output (+ sending it)
*/
void translateToOutput(struct AddressableLED** sequence, unsigned short sequenceLength);

/*
 * Function that will ask output address and port and will fetch size from output
 */
int getOutputInformations();

unsigned short getRowSize();
unsigned short getColumnSize();

#endif
