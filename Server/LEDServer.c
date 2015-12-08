#include "LEDServer.h"

static unsigned short numberOfRows = 0, numberOfColumns = 0;

void serverClose()
{
	//outputCleanUp();
}


void translateToOutput(struct AddressableLED** sequence, unsigned short sequenceLength)
{
	for (int index = 0; index < sequenceLength; index ++)
	{
		if (sequence[index]->address.row < numberOfRows && sequence[index]->address.column < numberOfColumns)
		{
			//setLed( (sequence[index]->address.row * numberOfColumns + sequence[index]->address.column), sequence[index]->ledValue);
		}
	} 
	//printLedGrid();
}

unsigned short getRowSize()
{
	return numberOfRows;
}
unsigned short getColumnSize()
{
	return numberOfColumns;
}


