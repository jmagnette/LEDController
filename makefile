CC = gcc

IN = Input/
OUT = Output/
MID = Server/
LIB = MainLib/lib/
INC = MainLib/include/
MOD = MainLib/modules/

CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -Iinclude/ -L./$(LIB)

O_FILES = *.o $(IN)*.o $(OUT)*.o $(MID)*.o


.PHONY: clean



DEBUG?=0

ifeq ($(DEBUG),1)
	CFLAGS += -g
endif



all: LEDParser LEDSim clean


LEDSTM32: $(OUT)LEDConsoleOut.o $(OUT)WS2812b_STM32F4Disc.c $(LIB)libpicotcp.a
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) -lpicotcp

LEDParser: LEDTest.o $(MID)LEDServer.o $(IN)LEDInput.o $(MID)JSONToLeds.o $(LIB)libjsmn.a $(LIB)libpicotcp.a $(MOD)pico_dev_vde.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) -ljsmn -lpicotcp -lvdeplug
	
LEDSim: $(OUT)LEDConsoleOut.o $(OUT)SimulatedLED.c $(LIB)libpicotcp.a $(MOD)pico_dev_vde.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) -lpicotcp -lvdeplug

	
LEDTest.o: LEDTest.c $(MID)LEDServer.h $(IN)LEDInput.h

LEDServer.o: $(MID)LEDServer.c $(MID)LEDServer.h $(OUT)LEDConsoleOut.h $(INC)Byte.h $(INC)LedRGB.h $(LIB)libpicotcp.a
	
LEDInput.o: $(IN)LEDInput.c $(IN)LEDInput.h $(MID)LEDServer.h $(INC)Byte.h $(INC)LedRGB.h
	
JSONToLeds.o: $(MID)JSONToLeds.c $(MID)JSONToLeds.h $(LIB)libjsmn.a $(INC)LedRGB.h
	
LEDConsoleOut.o: $(OUT)LEDConsoleOut.c $(OUT)LEDConsoleOut.h $(INC)Byte.h $(INC)LedRGB.h $(LIB)libpicotcp.a
	
clean:
	rm -rf $(O_FILES)
	
