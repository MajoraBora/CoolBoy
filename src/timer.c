#include "timer.h"
#include "gameboy.h"
#include "bitUtils.h"

const int frequencies[4] = {
	4096,
	262144,
	65536,
	16384
};

bool isTimerEnabled(struct gameboy * gameboy)
{
	uint8_t bit = readByte(gameboy, TMC);
	return isBitSet(bit, TIMER_ENABLED_BIT);
}
