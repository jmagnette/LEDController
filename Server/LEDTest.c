#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../Output/LEDConsoleOut.h"
#include "../MainLib/Byte.h"
#include "../MainLib/LedRGB.h"
#include "../Input/LEDInput.h"

int chooseInput();
void exampleMenu();
void manualMenu();

int main ()
{
	system("clear");
	printf("Hello, welcome to our LED test program.\n");
	
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

	system("clear");
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
		printf("- Set up LED line (0)\n");
		printf("- Change color of one LED (1)\n");
		printf("- Change color of the full line (2)\n");
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
					printf("How many LEDs do you want?");
					unsigned short numberOfLeds = 0;
					fgets(userInput, sizeof(userInput), stdin);
					sscanf(userInput, "%hu", &numberOfLeds);
					setMaxLineLength(numberOfLeds);
					break;
					
				case 1 :
					if (getMaxLineLength() == 0)
						printf("Please, first set the line\n");
					else
					{
						printf("Enter address and RGB value to change\n");
						printf("(Format : addr red green blue) :");	
					}	
						
					break;
					
				case 2 :
					if (getMaxLineLength() == 0)
						printf("Please, first set the line\n");
					else
					{
						printf("Enter RGB value to change to\n");
						printf("(Format : red green blue) :");	
						
						// printOneColourLine(Struct RGB)
					}
					break;
			}
		}
	}
	
	
}









