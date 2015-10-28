#include "bitUtils.h"

bool isBitSet(uint8_t data, uint8_t byte)
{
	return ((data >> byte) & 1) ? true : false;
}

