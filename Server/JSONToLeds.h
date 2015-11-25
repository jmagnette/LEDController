#ifndef JSONTOLEDS_H
#define JSONTOLEDS_H
#include "../MainLib/LedRGB.h"
#include "../MainLib/jsmn.h"
#include <stdio.h>

void extractRGBFromFile(*struct LEDSequence extractedSequences, char* jSONFilePath);
void extractRGBFromString(*struct LEDSequence extractedSequences, char* jSONString); 

#endif
