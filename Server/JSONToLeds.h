#ifndef JSONTOLEDS_H
#define JSONTOLEDS_H
#include "../MainLib/LedRGB.h"
#include "../MainLib/jsmn.h"
#include <stdio.h>

enum jsonToLedErr
{
	NO_FILE_FOUND = -1,
	INVALID_JSON = -2,
	ALLOCATION_ERROR = -3
};

/*
	Function that will look for a file at the given path, handle the JSON code and will translate it to a LEDSequence array.
	The result array will be given back through extractedSequences that will point to the first sequence element. extractedSequences have to be allocated by the caller. Inner reallocation will be done if needed.
	Will give back a pointer to null if something goes wrong.
	returns 0 if all OK, a jsonToLedErr enum value otherwise.
	
	Note: the sequences array will be terminated with a null pointer
*/
int extractRGBFromFile(struct LEDSequence extractedSequences[], char* jSONFilePath);

/*
	Function that will handle the JSON code and will translate it to a LEDSequence array.
	The result array will be given back through extractedSequences that will point to the first sequence element. extractedSequences have to be allocated by the caller. Inner reallocation will be done if needed.
	Will give back a pointer to null if something goes wrong.
	returns 0 if all OK, a jsonToLedErr enum value otherwise.
	
	Note: the sequences array will be terminated with a null pointer
*/
int extractRGBFromString(struct LEDSequence extractedSequences[], char* jSONString);

#endif
