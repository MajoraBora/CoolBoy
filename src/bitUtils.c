#include "../include/bitUtils.h"
#include <stdio.h>

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

//for debugging. Only use with 8 bits, cba putting validation in
void printBinFromDec(uint8_t dec)
{
	int bitCount = 8;
	int binaryNumber[bitCount];
	int i = 1;
	long int quotient = dec;
	while (quotient != 0){
		binaryNumber[i++] = quotient % 2;
		quotient /= 2;
	}

	for (int j = i - 1; j > 0; j--){
		printf("%d", binaryNumber[j]);
	}
	printf("\n");
}
