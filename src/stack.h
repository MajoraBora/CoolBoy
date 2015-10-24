#ifndef STACK_H
#define STACK_H

#include "gameboy.h"
#include <stdint.h>

void pushWordToStack(struct gameboy * gameboy, uint16_t word);
uint16_t popWordFromStack(struct gameboy * gameboy);

#endif
