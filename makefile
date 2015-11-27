CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = 
EXEC = LEDTest
IN = Input/
OUT = Output/
MID = Server/
LIB = MainLib/

all: $(EXEC) clean

LEDTest: LEDTest.o LEDServer.o LEDInput.o LEDConsoleOut.o JSONToLeds.o
	$(CC) $^ -o $@ $(LDFLAGS)
	
LEDTest.o: LEDTest.c $(MID)LEDServer.h $(IN)LEDInput.h
	$(CC) -c $< -o $@ $(CFLAGS)

LEDServer.o: $(MID)LEDServer.c $(MID)LEDServer.h $(OUT)LEDConsoleOut.h $(LIB)Byte.h $(LIB)LedRGB.h
	$(CC) -c $< -o $@ $(CFLAGS)
	
LEDInput.o: $(IN)LEDInput.c $(IN)LEDInput.h $(MID)LEDServer.h $(LIB)Byte.h $(LIB)LedRGB.h
	$(CC) -c $< -o $@ $(CFLAGS)
	
LEDConsoleOut.o: $(OUT)LEDConsoleOut.c $(OUT)LEDConsoleOut.h $(LIB)Byte.h $(LIB)LedRGB.h
	$(CC) -c $< -o $@ $(CFLAGS)
	
JSONToLeds.o: $(MID)JSONToLeds.c $(MID)JSONToLeds.h $(LIB)libjsmn.a $(LIB)LedRGB.h
	$(CC) -c $< -o $@ $(CFLAGS) -ljsmn
	
clean:
	rm -rf *.o
