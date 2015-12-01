#include "JSONToLeds.h"

struct LEDSequence **extractedSequences = NULL;
unsigned short isInitialized = 0;

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

static int addNewSequence(unsigned short delay)
{
	return 0;
}

static int appendToCurrentSequence(struct AddressableLED* ledValues)
{
	return 0;
}

int extractRGBFromFile(char* jSONFilePath)
{
	FILE *jSONFile;
	char *buffer;
	long length;

	jSONFile = fopen(jSONFilePath,"r");
	if (NULL == jSONFile)
		return OPEN_FILE_ERROR;

	fseek(jSONFile, 0L, SEEK_END);
	length = ftell(jSONFile);
	fseek(jSONFile, 0L, SEEK_SET);

	buffer = malloc((length + 1) * sizeof(char));
	if (NULL == buffer)
		return ALLOCATION_ERROR;

	fread(buffer, sizeof(char), length, jSONFile);
	buffer[length] = '\0';

	if (0 != fclose(jSONFile) )
	{
		free(buffer);
		return CLOSE_FILE_ERROR;
	}

	int result = extractRGBFromString(buffer);

	if (0 != result)
	{
		free(buffer);
	}
	return result;
}

int extractRGBFromString(char* jSONString)
{
	if (0 == isInitialized)
		return LEDSEQUENCES_NOT_INITIALIZED;

	int nbrOfToken;
	jsmn_parser parser;
	jsmntok_t *tokenList;

	jsmn_init(&parser);

	nbrOfToken = jsmn_parse(&parser, jSONString, strlen(jSONString), NULL, 0);
	if (nbrOfToken < 0)
		return INVALID_JSON;

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
		fprintf(stderr,"Expecting object !\n");
		return INVALID_JSON;
	}

	for (int tokenIndex = 1; tokenIndex < nbrOfToken; ++tokenIndex)
	{
		if (0 == strTokenCompare(jSONString, &tokenList[tokenIndex], "Name"))
		{
			printf("- Name: %.*s\n", tokenList[tokenIndex + 1].end-tokenList[tokenIndex + 1].start,
					jSONString + tokenList[tokenIndex + 1].start);
			++tokenIndex;
		}
		else if (0 == strTokenCompare(jSONString, &tokenList[tokenIndex], "Size"))
		{
			printf("- Size: %.*s\n", tokenList[tokenIndex + 1].end-tokenList[tokenIndex + 1].start,
								jSONString + tokenList[tokenIndex + 1].start);
			++tokenIndex;
		}
		else if (0 == strTokenCompare(jSONString, &tokenList[tokenIndex], "Seq"))
		{
			printf("Beginning Sequence parsing...\n");
			if (JSMN_ARRAY != tokenList[tokenIndex + 1].type)
			{
				continue; //we expect to have an array after Seq
			}
			++tokenIndex;

			printf("Number of child in SEQ array : %d \n", tokenList[tokenIndex].size);
			for (int seqIndex = 0; seqIndex < tokenList[tokenIndex].size; ++seqIndex)
			{
				if (JSMN_OBJECT != tokenList[tokenIndex + 1].type)
				{
					fprintf(stderr, "Incorrect JSON format at %d character, was expecting object.\n", tokenList[tokenIndex + 1].start);
					free(tokenList);
					freeLEDSequences();
					return INVALID_JSON;
				}
				else
				{
					tokenIndex += 2;
					if (0 == strTokenCompare(jSONString, &tokenList[tokenIndex], "Delay"))
					{
						++tokenIndex;
						//To continue
					}
					else
					{
						fprintf(stderr, "Incorrect JSON format at %d character, was expecting Delay.\n", tokenList[tokenIndex].start);
						free(tokenList);
						freeLEDSequences();
						return INVALID_JSON;
					}

				}
			}

		}
	}


	free(tokenList);
	tokenList = NULL;
	isInitialized = 0;

	return 0;
}

struct LEDSequence** initLEDSequences()
{
	freeLEDSequences();

	extractedSequences = malloc(2 * sizeof(struct LEDSequence*));
	if (NULL != extractedSequences)
	{
		extractedSequences[0]= malloc(sizeof(struct AddressableLED));
		extractedSequences[0]->delay = 0;
		extractedSequences[0]->ledToChange = NULL;
		extractedSequences[1] = NULL;
		isInitialized = 1;
	}

	return extractedSequences;
}

void freeLEDSequences()
{
	if (extractedSequences)
	{
		int index = 0;
		while(NULL != extractedSequences[index]->ledToChange)
		{
			free(extractedSequences[index]->ledToChange);
			++index;
		}
		free(extractedSequences);
		extractedSequences = NULL;
	}

}
