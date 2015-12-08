#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Input/LEDInput.h"
#include "Server/LEDServer.h"


int main ()
{
	system("clear");
	printf("Hello, welcome to our LED test program.\n");
	
	int rslt = beginConsoleInput();
	
	serverClose();
	
	return rslt;
}






