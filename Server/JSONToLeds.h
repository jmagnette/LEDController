#ifndef JSONTOLEDS_H
#define JSONTOLEDS_H
#include "../MainLib/LedRGB.h"
#include "../MainLib/jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum jsonToLedErr
{
	OPEN_FILE_ERROR = -1,
	CLOSE_FILE_ERROR = -2,
	ALLOCATION_ERROR = -3,
	INVALID_JSON = -4,
	LEDSEQUENCES_NOT_INITIALIZED = -5
};

/*
	Function that will look for a file at the given path, handle the JSON code and will translate it to a LEDSequence array.
	The result array will be given back through extractedSequences that will point to the first sequence element.
	initLEDSequences() has to be called first each time.
	returns 0 if all OK, a jsonToLedErr enum value otherwise.
	
	Note: the sequences array will be and has to be terminated with a null pointer
*/
int extractRGBFromFile(char* jSONFilePath);

/*
	Function that will handle the JSON code and will translate it to a LEDSequence array.
	The result array will be stored in the inner extractedSequences that will point to the first sequence element.
	initLEDSequences() has to be called first each time.
	returns 0 if all OK, a jsonToLedErr enum value otherwise.
	
	Note: the sequences array will be and has to be terminated with a null pointer
*/
int extractRGBFromString(char* jSONString);

/*
 * Function that will handle the initialization of the inner LEDSequences array
 * Will return a pointer to its first 'object'
 * The array will be NULL terminated and will contains one empty element.
 *
 * Will return a NULL pointer if something goes wrong
 *
 * !!Beware!! dynamic allocation will be used ! Please call freeLEDSequences() when done with the LEDSequences
 *
 */
struct LEDSequence** initLEDSequences();

/*
 * Function that will handle the freeing of the memory used by the inner extractedSequences array
 *
 */
void freeLEDSequences();

#endif
