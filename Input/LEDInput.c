#include "LEDInput.h"

int chooseInput();
void exampleMenu();
void manualMenu();
void printBlinkingExample();
void printCyclingExample();

int beginConsoleInput()
{
	int userchoice = 0;
	while ((userchoice = chooseInput() ) >= 0)
	{
		/*according to the result of chooseInput, execute next task*/
		switch (userchoice)
		{
			case EXAMPLES :
				exampleMenu();
				break;
			
			case MANUAL :
				// launch method for manual input;
				break;
				
			default :
				fprintf(stderr,"Wrong user choice! closing program...\n");
				return 1;
		}
	}
	return 0;	
}

int chooseInput()
{
	int inputChoice = 0;
	char userInput[100];

	printf("###############################################\n");
	printf("Please choose which type of input you are going to use.\n");
	printf("the choices are the following:\n");

	/*List all the possible input*/
	printf("- Use examples (0)\n");
	printf("- Manual input (1)\n");
	printf("- Exit program (any negative value)\n");
	printf("Which one will it be ?");
	fgets(userInput, sizeof(userInput), stdin);
	sscanf(userInput, "%d", &inputChoice);	
	while (inputChoice >= MAXLEDINPUT)
	{
		printf("please enter a correct value!");
		fgets(userInput, sizeof(userInput), stdin);
		sscanf(userInput, "%d", &inputChoice);
	}
	
	return inputChoice;
}

void exampleMenu()
{
	char userInput[100];
	int inputChoice = 0;
	
	while (inputChoice >= 0)
	{
		printf("___Examples____________________________________\n");
		printf("Which example do you want to launch?\n");
		printf("the choices are the following:\n");
		printf("- Blinking Led (0)\n");
		printf("- Cycling Led (1)\n");
		printf("- Back to menu (any negative value)\n");
	
		fgets(userInput, sizeof(userInput), stdin);
		sscanf(userInput, "%d", &inputChoice);	
		
		if (inputChoice > 1)
			printf("please enter a correct value!\n");
		else if (inputChoice < 0)
			break;
		else
		{
			switch (inputChoice)
			{
				case 0 :
					printBlinkingExample();
					break;
					
				case 1 :
					printCyclingExample();
					break;
			}
		}
	}

}

void manualMenu()
{
	char userInput[100];
	int inputChoice = 0;
	
	while (inputChoice >= 0)
	{
		printf("___Manual_____________________________________\n");
		printf("What do you want to do?\n");
		printf("- Change color of one LED (0)\n");
		printf("- Change color of the full line (1)\n");
		printf("- Back to menu (any negative value)\n");
		
		fgets(userInput, sizeof(userInput), stdin);
		sscanf(userInput, "%d", &inputChoice);
		
		if (inputChoice > 2)
			printf("please enter a correct value!\n");
		else if (inputChoice < 0)
			break;
		else
		{
			switch (inputChoice)
			{
				case 0 :
					printf("Enter address and RGB value to change\n");
					printf("(Format : addr red green blue) :");	
					break;
					
				case 1 :
					printf("Enter RGB value to change to\n");
					printf("(Format : red green blue) :");	
						
					// printOneColourLine(Struct RGB)
					break;
			}
		}
	}
}


void printBlinkingExample()
{	
	unsigned short nbOfFirstInput = 1;
	struct AddressableLED firstInput[] =
	{
		{ {0,1} , {255,100,0} }
	};
	
	unsigned short nbOfSecondInput = 1;
	struct AddressableLED secondInput[] =
	{
		{ {0,1} , {0,0,0} }
	};
	
	for(int index = 0; index < 10; index ++)
	{
		translateToOutput(firstInput, nbOfFirstInput);
		nanosleep(&(struct timespec){0,500*1000000},NULL);
		translateToOutput(secondInput, nbOfSecondInput);
		nanosleep(&(struct timespec){0,500*1000000},NULL);
	}
}

void printCyclingExample()
{
	unsigned short row = 2;
	unsigned short column = 5;
	
	struct RGB initialRGB = { 0, 0, 0};
	struct RGB loadingRGB = { 165, 242, 243};
	
	struct AddressableLED sequence[2]; 
	
	for(int loopNumber = 0; loopNumber < 5; ++loopNumber)
	{
		for(unsigned short index = 0; index < (row * column) ; ++index)
		{
			if (0 == index)
			{
				sequence[0].address.row = row - 1;
				sequence[0].address.column = column -1;
			}
			else
			{
				sequence[0].address.row = (index - 1) / column;
				sequence[0].address.column = (index - 1) % column;
			}
			
			sequence[0].ledValue = initialRGB;
			sequence[1].address.row = index / column;
			sequence[1].address.column = index % column;
			sequence[1].ledValue = loadingRGB;
			
			translateToOutput(sequence, 2);
			nanosleep(&(struct timespec){0,200*1000000},NULL);
		}
	}	
}
