#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include <stdint.h>
#include <stdbool.h>

bool isBitSet(uint8_t data, uint8_t bit);
void setBit(uint8_t * data, uint8_t bit, bool state);
uint8_t getBit(int data, int position);
void printBinFromDec(uint8_t dec);
#endif
