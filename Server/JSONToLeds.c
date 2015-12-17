#include "JSONToLeds.h"

static struct LEDSequence **extractedSequences = NULL;
static unsigned short isInitialized = 0;
static unsigned int numberOfSequences = 0;


static int strTokenCompare(const char *jSONString, jsmntok_t *token, const char *cprString)
{
	if (token->type == JSMN_STRING && ((int) strlen(cprString)) == token->end - token->start &&
				strncmp(jSONString + token->start, cprString, token->end - token->start) == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

static int addNewSequence(int delay)
{
	isInitialized = 0;

	++numberOfSequences;

	struct LEDSequence **tmpSequences;
	tmpSequences = realloc(extractedSequences, (numberOfSequences + 1) * sizeof(struct LEDSequence*));
	if (NULL != tmpSequences)
	{
		extractedSequences = tmpSequences;
		tmpSequences = NULL;
		extractedSequences[numberOfSequences - 1] = malloc(sizeof(struct AddressableLED));
		if (NULL != extractedSequences[numberOfSequences - 1])
		{
			extractedSequences[numberOfSequences - 1]->delay = (unsigned short) delay;
			extractedSequences[numberOfSequences - 1]->sequenceLength = 0;
			extractedSequences[numberOfSequences - 1]->ledToChange = NULL;
			extractedSequences[numberOfSequences] = NULL;

			return 0;
		}
	}

	fprintf(stderr,"Not enough memory, cannot allocate more.\n");
	freeLEDSequences();
	return ALLOCATION_ERROR;
}

static int appendToCurrentSequence(const struct AddressableLED ledValues)
{
	extractedSequences[numberOfSequences - 1]->sequenceLength += 1;
	int numberOfLEDValues = extractedSequences[numberOfSequences - 1]->sequenceLength;

	struct AddressableLED **tmpLedValues;
	tmpLedValues = realloc(extractedSequences[numberOfSequences - 1]->ledToChange, (numberOfLEDValues + 1) * sizeof(struct AddressableLED*));

	if (NULL != tmpLedValues)
	{
		extractedSequences[numberOfSequences - 1]->ledToChange = tmpLedValues;
		extractedSequences[numberOfSequences - 1]->ledToChange[numberOfLEDValues - 1] = malloc(sizeof(struct AddressableLED));
		if (NULL != extractedSequences[numberOfSequences - 1]->ledToChange[numberOfLEDValues - 1])
		{
			extractedSequences[numberOfSequences - 1]->ledToChange[numberOfLEDValues - 1]->address.row = ledValues.address.row;
			extractedSequences[numberOfSequences - 1]->ledToChange[numberOfLEDValues - 1]->address.column = ledValues.address.column;
			extractedSequences[numberOfSequences - 1]->ledToChange[numberOfLEDValues - 1]->ledValue.red = ledValues.ledValue.red;
			extractedSequences[numberOfSequences - 1]->ledToChange[numberOfLEDValues - 1]->ledValue.green = ledValues.ledValue.green;
			extractedSequences[numberOfSequences - 1]->ledToChange[numberOfLEDValues - 1]->ledValue.blue = ledValues.ledValue.blue;

			extractedSequences[numberOfSequences - 1]->ledToChange[numberOfLEDValues] = NULL;
			return 0;
		}

	}

	fprintf(stderr,"Not enough memory, cannot allocate more.\n");
	freeLEDSequences();
	return ALLOCATION_ERROR;
}

static void incorrectJSONHandler(char* expectedType, jsmntok_t **tokenList, int indexFaultyToken)
{
	fprintf(stderr, "Incorrect JSON format at character %d, was expecting %s.\n", (*tokenList)[indexFaultyToken].start, expectedType);
	free(*tokenList);
	*tokenList = NULL;
	freeLEDSequences();
}

int extractRGBFromFile(char* jSONFilePath, struct LEDSequence*** outSequences)
{
	FILE *jSONFile;
	char *buffer;
	long length;

	jSONFile = fopen(jSONFilePath,"r");
	if (NULL == jSONFile)
	{
		fprintf(stderr,"Cannot open %s \nError number : %d\n",jSONFilePath, errno);
		return OPEN_FILE_ERROR;
	}


	fseek(jSONFile, 0L, SEEK_END);
	length = ftell(jSONFile);
	fseek(jSONFile, 0L, SEEK_SET);

	buffer = malloc((length + 1) * sizeof(char));
	if (NULL == buffer)
	{
		fprintf(stderr,"Not enough memory, cannot allocate more.\n");
		fclose(jSONFile);
		return ALLOCATION_ERROR;
	}


	fread(buffer, sizeof(char), length, jSONFile);
	buffer[length] = '\0';

	if (0 != fclose(jSONFile) )
	{
		fprintf(stderr,"Cannot close %s \nError number : %d\n",jSONFilePath, errno);
		free(buffer);
		buffer = NULL;
		return CLOSE_FILE_ERROR;
	}

	int result = extractRGBFromString(buffer, outSequences);

	if (0 != result)
	{
		free(buffer);
		buffer = NULL;
	}
	return result;
}

int extractRGBFromString(char* jSONString, struct LEDSequence*** outSequences)
{
	if (0 == isInitialized)
	{
		fprintf(stderr,"The sequences were not initialized, please call 'initLEDSequences()' first.\n");
		return LEDSEQUENCES_NOT_INITIALIZED;
	}

	int nbrOfToken;
	jsmn_parser parser;
	jsmntok_t *tokenList;

	jsmn_init(&parser);

	nbrOfToken = jsmn_parse(&parser, jSONString, strlen(jSONString), NULL, 0);
	if (nbrOfToken < 0)
	{
		fprintf(stderr,"JSON error : %d\n",nbrOfToken);
		return INVALID_JSON;
	}

	jsmn_init(&parser);
	tokenList = malloc((nbrOfToken)* sizeof(jsmntok_t));
	nbrOfToken = jsmn_parse(&parser, jSONString, strlen(jSONString), tokenList, nbrOfToken);

	if (nbrOfToken < 0)
	{
		fprintf(stderr,"JSON error : %d\n",nbrOfToken);
		return INVALID_JSON;
	}

	if (nbrOfToken < 1 || tokenList[0].type != JSMN_OBJECT)
	{
		fprintf(stderr,"Expecting starting object in file !\n");
		return INVALID_JSON;
	}

	for (int tokenIndex = 1; tokenIndex < nbrOfToken; ++tokenIndex)
	{
		if (0 == strTokenCompare(jSONString, &tokenList[tokenIndex], "Name"))
		{
			printf("- Name: %.*s\n", tokenList[tokenIndex + 1].end-tokenList[tokenIndex + 1].start,
					jSONString + tokenList[tokenIndex + 1].start);
			++tokenIndex;
			// Do something with the name ?
		}
		else if (0 == strTokenCompare(jSONString, &tokenList[tokenIndex], "Size"))
		{
			printf("- Size: %.*s\n", tokenList[tokenIndex + 1].end-tokenList[tokenIndex + 1].start,
								jSONString + tokenList[tokenIndex + 1].start);
			++tokenIndex;
			// Do something with the size ?
		}
		else if (0 == strTokenCompare(jSONString, &tokenList[tokenIndex], "Seq"))
		{
			printf("Beginning Sequence parsing...\n");
			if (JSMN_ARRAY != tokenList[tokenIndex + 1].type)
			{
				continue; //we expect to have an array after Seq
			}
			++tokenIndex;

			int nbrOfSeq = tokenList[tokenIndex].size;
			for (int seqIndex = 0; seqIndex < nbrOfSeq; ++seqIndex)
			{
				if (JSMN_OBJECT != tokenList[tokenIndex + 1].type)
				{
					incorrectJSONHandler("Object", &tokenList, tokenIndex + 1);
					return INVALID_JSON;
				}
				else
				{
					tokenIndex += 2;
					if (0 == strTokenCompare(jSONString, &tokenList[tokenIndex], "Delay")
							&& JSMN_PRIMITIVE == tokenList[tokenIndex + 1].type)
					{
						++tokenIndex;
						int nbrOfChar = tokenList[tokenIndex].end - tokenList[tokenIndex].start;
						char delayString[nbrOfChar + 1];
						strncpy(delayString, jSONString + tokenList[tokenIndex].start, nbrOfChar);
						delayString[nbrOfChar] = '\0';
						int result = addNewSequence(atoi(delayString));
						if (result != 0)
						{
							free(tokenList);
							tokenList = NULL;
							return result;
						}

						++tokenIndex;
						if (0 == strTokenCompare(jSONString, &tokenList[tokenIndex], "Led")
								&& JSMN_ARRAY == tokenList[tokenIndex + 1].type)
						{
							++tokenIndex;

							struct AddressableLED tmpAdrrLed = { {0,0} , {0,0,0} };
							int nbrOfLeds = tokenList[tokenIndex].size;
							for (int ledIndex = 0; ledIndex < nbrOfLeds; ++ledIndex)
							{
								++tokenIndex;
								if (JSMN_OBJECT != tokenList[tokenIndex].type)
								{
									incorrectJSONHandler("Object", &tokenList, tokenIndex);
									return INVALID_JSON;
								}
								++tokenIndex;


								if (0 != strTokenCompare(jSONString, &tokenList[tokenIndex], "Addr")
										|| JSMN_STRING != tokenList[tokenIndex + 1].type
										|| 0 != strTokenCompare(jSONString, &tokenList[tokenIndex + 2], "R")
										|| JSMN_PRIMITIVE != tokenList[tokenIndex + 3].type
										|| 0 != strTokenCompare(jSONString, &tokenList[tokenIndex + 4], "G")
										|| JSMN_PRIMITIVE != tokenList[tokenIndex + 5].type
										|| 0 != strTokenCompare(jSONString, &tokenList[tokenIndex + 6], "B")
										|| JSMN_PRIMITIVE != tokenList[tokenIndex + 7].type  )
								{
									incorrectJSONHandler("LED value (in the form Addr:String - Red:Byte - Green:Byte - Blue:Byte)", &tokenList, tokenIndex);
									return INVALID_JSON;
								}

								// extraction of the address
								nbrOfChar = tokenList[tokenIndex + 1].end - tokenList[tokenIndex + 1].start;
								char addrString[nbrOfChar + 1];
								strncpy(addrString, jSONString + tokenList[tokenIndex + 1].start, nbrOfChar);
								addrString[nbrOfChar] = '\0';
								char *subString = strtok(addrString, "x");
								if (NULL == subString)
								{
									incorrectJSONHandler("Addr with form : axb or ALL", &tokenList, tokenIndex + 1);
									return INVALID_JSON;
								}
								if (0 == strcmp("ALL", subString))
								{
									// Reset is assigned to address 65535 x 65535 (that cannot be used for something else (and should not))
									tmpAdrrLed.address.row = USHRT_MAX;
									tmpAdrrLed.address.column = USHRT_MAX;
								}
								else
								{
									tmpAdrrLed.address.row = (unsigned short) atoi(subString);

									subString = strtok(NULL, "x");
									if (NULL == subString)
									{
										incorrectJSONHandler("Addr with form : axb", &tokenList, tokenIndex + 1);
										return INVALID_JSON;
									}
									tmpAdrrLed.address.column = (unsigned short) atoi(subString);
								}

								//extraction of the Red value
								nbrOfChar = tokenList[tokenIndex + 3].end - tokenList[tokenIndex + 3].start;
								char redString[nbrOfChar + 1];
								strncpy(redString, jSONString + tokenList[tokenIndex + 3].start, nbrOfChar);
								redString[nbrOfChar] = '\0';
								tmpAdrrLed.ledValue.red = (Byte) atoi(redString);

								//extraction of the Green value
								nbrOfChar = tokenList[tokenIndex + 5].end - tokenList[tokenIndex + 5].start;
								char greenString[nbrOfChar + 1];
								strncpy(greenString, jSONString + tokenList[tokenIndex + 5].start, nbrOfChar);
								greenString[nbrOfChar] = '\0';
								tmpAdrrLed.ledValue.green = (Byte) atoi(greenString);

								//extraction of the Blue value
								nbrOfChar = tokenList[tokenIndex + 7].end - tokenList[tokenIndex + 7].start;
								char blueString[nbrOfChar + 1];
								strncpy(blueString, jSONString + tokenList[tokenIndex + 7].start, nbrOfChar);
								blueString[nbrOfChar] = '\0';
								tmpAdrrLed.ledValue.blue = (Byte) atoi(blueString);

								result = appendToCurrentSequence(tmpAdrrLed);
								if (0 != result)
								{
									free(tokenList);
									tokenList = NULL;
									return result;
								}
								tokenIndex += 7;

							}
						}
						else
						{
							incorrectJSONHandler("Led/Array", &tokenList, tokenIndex);
							return INVALID_JSON;
						}

					}
					else
					{
						incorrectJSONHandler("Delay", &tokenList, tokenIndex);
						return INVALID_JSON;
					}

				}
			}

		}
	}

	*outSequences = extractedSequences;
	free(tokenList);
	tokenList = NULL;
	isInitialized = 0;

	return 0;
}

void initLEDSequences()
{
	freeLEDSequences();

	extractedSequences = malloc(sizeof(struct LEDSequence*));
	if (NULL != extractedSequences)
	{
		extractedSequences[0] = NULL;
		isInitialized = 1;
	}
}

void freeLEDSequences()
{
	if (extractedSequences)
	{
		int index = 0;
		while(NULL != extractedSequences[index])
		{
			free(extractedSequences[index]->ledToChange);
			++index;
		}
		free(extractedSequences);
		extractedSequences = NULL;
	}
	numberOfSequences = 0;

}
