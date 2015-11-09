#include "../include/bitUtils.h"

bool isBitSet(uint8_t data, uint8_t bit)
{
	return ((data >> bit) & 1) ? true : false;
}

void setBit(uint8_t * data, uint8_t bit, bool state)
{
	if (state){
		*data |= 1 << bit;
	}
	else {
		*data &= ~(1 << bit);
	}
}

uint8_t getBit(int data, int position)
{
	uint8_t mask = 1 << position;
	return (data & mask) ? 1 : 0;
}
