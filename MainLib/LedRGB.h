#ifndef LEDRGB_H
#define LEDRGB_H
#include "Byte.h"
struct RGB{
	Byte red;
	Byte green;
	Byte blue;
};

struct Address{
	unsigned short row;
	unsigned short column;
};

struct AddressableLED{
	struct Address address;
	struct RGB ledValue;
};

struct LEDSequence{
	unsigned short delay;
	struct AddressableLED ledToChange[];
};
#endif
