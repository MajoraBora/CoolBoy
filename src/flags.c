#include "flags.h"

bool isFlagSet(struct gameboy * gameboy, enum flag flag)
{
	uint8_t f = gameboy->cpu.f;
	return ((f >> flag) & 1) ? true : false;
}

void setFlag(struct gameboy * gameboy, enum flag flag, bool state)
{
	if (state){
		gameboy->cpu.f |= 1 << flag;
	}
	else {
		gameboy->cpu.f &= ~(1 << flag);
	}
}
