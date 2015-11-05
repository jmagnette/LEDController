CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = 
EXEC = LEDTest
IN = Input/
OUT = Output/
MID = Server/
LIB = MainLib/

all: $(EXEC) clean

LEDTest: LEDTest.o LEDConsoleOut.o
	$(CC) $^ -o $@ $(LDFLAGS)
	
LEDTest.o: $(MID)LEDTest.c $(OUT)LEDConsoleOut.h $(LIB)Byte.h $(LIB)LedRGB.h $(IN)LEDInput.h
	$(CC) -c $< -o $@ $(CFLAGS)
	
LEDConsoleOut.o: $(OUT)LEDConsoleOut.c $(OUT)LEDConsoleOut.h $(LIB)Byte.h $(LIB)LedRGB.h
	$(CC) -c $< -o $@ $(CFLAGS)
	
clean:
	rm -rf *.o
