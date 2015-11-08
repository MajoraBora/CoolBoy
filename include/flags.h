#ifndef FLAGS_H
#define FLAGS_H

#include <stdbool.h>
#include "gameboy.h"

/*
Flags are contained in the F upper 4 bits of the F register
*/

enum flag {
	ZERO = 7,
	SUB = 6,
	HALF_CARRY = 5,
	CARRY = 4
};

bool isFlagSet(struct gameboy * gameboy, enum flag flag);
void setFlag(struct gameboy * gameboy, enum flag flag, bool state);

#endif
