#include "../include/timer.h"
#include "../include/gameboy.h"
#include "../include/bitUtils.h"

const int frequencies[4] = {
	4096,
	262144,
	65536,
	16384
};

static void handleTimerIncrement(struct gameboy * gameboy);
static void handleDividerRegister(struct gameboy * gameboy, int cycles);

void initialiseTimerCounter(struct gameboy * gameboy)
{
	int timerFreq = getTimerFrequency(gameboy);
	gameboy->timers.timerCounter = timerFreq;
}

bool isTimerEnabled(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, TMC);
	return isBitSet(byte, TIMER_ENABLED_BIT);
}

int getTimerFrequency(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, TMC);
	//frequency is first 2 bits of byte
	uint8_t lowerTwoBits = byte & 0x3;
	return frequencies[lowerTwoBits];
}

void updateTimers(struct gameboy * gameboy, int cycles)
{
	//for every (clockspeed/frequency) clock cycles, the timer should increment
	//itself once

	handleDividerRegister(gameboy, cycles);

	if (isTimerEnabled(gameboy)){
		//update the elapsed cpu cycles
		gameboy->timers.timerCounter -= cycles;
		//if the correct amount of cycles have passed
		if (gameboy->timers.timerCounter <= 0){
			//reset the counter, increment the timer.
			//the counters are used to increment the actual timer at the
			//correct time. The counters are NOT the timers.
			initialiseTimerCounter(gameboy);
			handleTimerIncrement(gameboy);
		}
	}
}

static void handleTimerIncrement(struct gameboy * gameboy)
{
	//if TIMA has reached 255 (ie is about to overflow)
	//reset it to value at TMA, then request a timer interrupt
	//else, increment the value at TIMA
	uint8_t tima = readByte(gameboy, TIMA);
	if (tima == OVERFLOW){
		int tma = readByte(gameboy, TMA);
		writeByte(gameboy, TIMA, tma);
		//timerInterrupt(gameboy);
	}
	else {
		writeByte(gameboy, TIMA, tima+1);
	}

}

static void handleDividerRegister(struct gameboy * gameboy, int cycles)
{
	//same sort of deal as the timer
	gameboy->timers.dividerCounter += cycles;
	if (gameboy->timers.dividerCounter >= OVERFLOW){
		gameboy->timers.dividerCounter = 0;
		gameboy->memory.mem[DIV_REG]++;
	}
}
