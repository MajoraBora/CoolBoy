#include "flags.h"
#include "bitUtils.h"

bool isFlagSet(struct gameboy * gameboy, enum flag flag)
{
	uint8_t f = gameboy->cpu.f;
	return isBitSet(f, flag);
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
