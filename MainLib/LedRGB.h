#ifndef LEDRGB_H
#define LEDRGB_H
#include "Byte.h"
struct RGB{
	Byte red;
	Byte green;
	Byte blue;
};

struct AddressableLED{
	unsigned short address;
	struct RGB ledValue;
};
#endif
