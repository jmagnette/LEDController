#ifndef LEDRGB_H
#define LEDRGB_H
#include "Byte.h"
struct RGB{
	Byte red;
	Byte green;
	Byte blue;
};

/*
	Structure designed to contain an address linked to a LED value
	(2D address)
*/
struct Address{
	unsigned short row;
	unsigned short column;
};

/*
	Structure that will contain the necessary data to handle a LED modification
*/
struct AddressableLED{
	struct Address address;
	struct RGB ledValue;
};

/*
	Structure used to contain a sequence of LEDs, to be seen as an 'image' at one given time.
	the delay value will be used to put some sleep between different sequences
*/
struct LEDSequence{
	unsigned short delay;
	struct AddressableLED ledToChange[];
};
#endif
