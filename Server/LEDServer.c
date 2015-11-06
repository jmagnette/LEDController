#include "LEDServer.h"

unsigned short numberOfRows = 0, numberOfColumns = 0;

void askForOutputLength()
{
 	printf("Before we go further, please enter the size of the LED grid you are going to use :\n");
	printf("(grid size formatted as (a x b), a = 1 for a line. Enter data as 'a b')\n");
	char userInput[100];
	
	short tmpRows = 0, tmpColumns = 0;
	fgets(userInput, sizeof(userInput), stdin);
	sscanf(userInput, "%hu %hu", &tmpRows, &tmpColumns);
	while (0 >= tmpRows || 0 >= tmpColumns)
	{
		printf("Please enter correct value for a and b : ");
		fgets(userInput, sizeof(userInput), stdin);
		sscanf(userInput, "%hu %hu", &tmpRows, &tmpColumns);
	}
	numberOfRows = tmpRows;
	numberOfColumns = tmpColumns;
	
	setMaxLineLength(numberOfRows * numberOfColumns);
}

void serverClose()
{
	outputCleanUp();
}


void translateToOutput(struct AddressableLED* sequence, unsigned short sequenceLength)
{
	for (int index = 0; index < sequenceLength; index ++)
	{
		if (sequence[index].address.row < numberOfRows && sequence[index].address.column < numberOfColumns)
		{
			setLed( (sequence[index].address.row * numberOfColumns + sequence[index].address.column), sequence[index].ledValue);
		}
	} 
	printLedGrid();
}
