#ifndef LEDRGB_H
#define LEDRGB_H
#include "Byte.h"

enum E_CommMessage
{
	LED_SIZE = 0x53495A45,
	LED_RESET = 0x52534554,
	LED_SET = 0x534C4544
};

enum E_CommError
{
	COM_SUCCESS = 0,
	COM_DEVICE_CANNOT_CREATE,
	COM_SOCKET_CANNOT_OPEN,
	COM_SOCKET_CANNOT_BIND,
	COM_SOCKET_CANNOT_LISTEN,
	COM_SOCKET_CANNOT_CONNECT,
	COM_SOCKET_ERROR
};

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
	unsigned short sequenceLength;
	struct AddressableLED **ledToChange;
};
#endif
