#include "LEDInput.h"

static int chooseInput();
static void exampleMenu();
static void manualMenu();
static void fileMenu();
static void networkMenu();
static void printBlinkingExample();
static void printCyclingExample();
static void printRandomFillingExample();

int beginConsoleInput()
{
	int userchoice = 0;
	while ((userchoice = chooseInput() ) >= 0)
	{
		/*according to the result of chooseInput, execute next task*/
		switch (userchoice)
		{
			case LED_EXAMPLES :
				exampleMenu();
				break;
			
			case LED_MANUAL :
				// launch method for manual input;
				break;
				
			case LED_FILE :
				fileMenu();
				break;
				
			case LED_NETWORK :
				// launch method for network input;
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
	printf("- File input (2)\n");
	printf("- Network input (3)\n");
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
		printf("- Random filling Led (2)\n");
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
					printBlinkingExample();
					break;
					
				case 1 :
					printCyclingExample();
					break;

				case 2 :
					printRandomFillingExample();
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

void fileMenu()
{
	char userInput[100];

	printf("___File input_________________________________\n");
	printf("Please enter the path of the desired file (with extension):\n");
	printf("(or enter empty string to go back to menu)\n");

	fgets(userInput, sizeof(userInput), stdin);
	char *filePath = malloc(strlen(userInput) * sizeof(char));
	if (NULL == filePath)
	{
		fprintf(stderr,"Not enough memory, cannot allocate more.\n");
		return;
	}
	sscanf(userInput, "%[^\n]", filePath);

	if (0 < strlen(filePath))
	{
		struct LEDSequence **sequencesToShow = NULL;
		initLEDSequences();

		if (0 == extractRGBFromFile(filePath, &sequencesToShow))
		{
			printf("The sequences were successfully extracted !\n");
			printf("How many time do you want to play those ?\n");
			printf("(entering a negative number or zero will get you back to the menu)\n");
			int numberOfLoops = 0;

			fgets(userInput, sizeof(userInput), stdin);
			sscanf(userInput, "%d", &numberOfLoops);

			int firstSequence = 1;
			for(int loopCounter = 0; loopCounter < numberOfLoops; loopCounter++)
			{
				int index = 0;
				while(NULL != sequencesToShow[index])
				{
					if(0 == firstSequence)
					{
						long delay = (sequencesToShow[index]->delay)*1000000;
						nanosleep(&(struct timespec){delay/1000000000,delay%1000000000},NULL);
					}
					else
					{
						firstSequence = 0;
					}

					translateToOutput(sequencesToShow[index]->ledToChange, sequencesToShow[index]->sequenceLength);
					++index;
				}
			}

			freeLEDSequences();
			sequencesToShow = NULL;
		}
	}
	free(filePath);
}

void networkMenu()
{

}

void printBlinkingExample()
{	
	unsigned short nbOfFirstInput = 1;
	struct AddressableLED* firstInput = &(struct AddressableLED){ {0,1} , {255,100,0} };
	
	unsigned short nbOfSecondInput = 1;
	struct AddressableLED* secondInput = &(struct AddressableLED){ {0,1} , {0,0,0}  };
	
	for(int index = 0; index < 10; index ++)
	{
		translateToOutput(&firstInput, nbOfFirstInput);
		nanosleep(&(struct timespec){0,500*1000000},NULL);
		translateToOutput(&secondInput, nbOfSecondInput);
		nanosleep(&(struct timespec){0,500*1000000},NULL);
	}
}

void printCyclingExample()
{
	unsigned short row = 2;
	unsigned short column = 5;
	
	struct RGB initialRGB = { 0, 0, 0};
	struct RGB loadingRGB = { 165, 242, 243};
	struct AddressableLED *sequence[2];
	sequence[0] = malloc(sizeof(struct AddressableLED));
	sequence[1] = malloc(sizeof(struct AddressableLED));

	for(int loopNumber = 0; loopNumber < 5; ++loopNumber)
	{
		for(unsigned short index = 0; index < (row * column) ; ++index)
		{
			if (0 == index)
			{
				sequence[0]->address.row = row - 1;
				sequence[0]->address.column = column -1;
			}
			else
			{
				sequence[0]->address.row = (index - 1) / column;
				sequence[0]->address.column = (index - 1) % column;
			}
			
			sequence[0]->ledValue = initialRGB;
			sequence[1]->address.row = index / column;
			sequence[1]->address.column = index % column;
			sequence[1]->ledValue = loadingRGB;
			
			translateToOutput(&sequence[0], 2);
			nanosleep(&(struct timespec){0,200*1000000},NULL);
		}
	}	
	free(sequence[0]);
	free(sequence[1]);
}

void printRandomFillingExample()
{
	time_t t;
	srand((unsigned) time(&t));

	unsigned short rows = getRowSize();
	unsigned short columns = getColumnSize();
	unsigned short nbrOfLEDsToGenerate = rows * columns;
	struct RGB randomColor = { 0,0,0 };
	struct RGB initialRGB = { 0, 0, 0};

	struct AddressableLED *sequence[2];
	sequence[0] = malloc(sizeof(struct AddressableLED));
	sequence[1] = malloc(sizeof(struct AddressableLED));

	while(0 < nbrOfLEDsToGenerate)
	{
		randomColor.red = rand() % 206 + 50;
		randomColor.green = rand() % 206 + 50;
		randomColor.blue = rand() % 206 + 50;

		for (unsigned short position = 0; position < nbrOfLEDsToGenerate; ++position )
		{
			if (0 != position)
			{
				sequence[0]->address.row = (position - 1) / columns;
				sequence[0]->address.column = (position - 1) % columns;
			}
			else
			{
				sequence[0]->address.row = position / columns;
				sequence[0]->address.column = position % columns;
			}
			sequence[0]->ledValue = initialRGB;
			sequence[1]->address.row = position / columns;
			sequence[1]->address.column = position % columns;
			sequence[1]->ledValue = randomColor;

			translateToOutput(&sequence[0], 2);
			nanosleep(&(struct timespec){0,100*1000000},NULL);
		}

		--nbrOfLEDsToGenerate;
	}

	free(sequence[0]);
	free(sequence[1]);
}
