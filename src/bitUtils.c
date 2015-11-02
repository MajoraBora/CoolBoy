#include "bitUtils.h"

bool isBitSet(uint8_t data, uint8_t byte)
{
	return ((data >> byte) & 1) ? true : false;
}

void setBit(uint8_t * data, uint8_t byte, bool state)
{
	if (state){
		*data |= 1 << byte;
	}
	else {
		*data &= ~(1 << byte);
	}
}

