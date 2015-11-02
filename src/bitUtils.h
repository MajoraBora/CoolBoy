#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include <stdint.h>
#include <stdbool.h>

bool isBitSet(uint8_t data, uint8_t byte);
void setBit(uint8_t * data, uint8_t byte, bool state);

#endif
