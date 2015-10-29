#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>

/*
Timer is located at 0xFF05.
Counts up at a frequency set by the timer controller at 0xFF07
When it overflows (ie > 255) it fires a timer interrupt and resets to the value
at 0xFF06, in the timer modulator
*/

#define DIV_REG 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TMC 0xFF07

#define TIMER_ENABLED_BIT 2

#define OVERFLOW 255

struct gameboy;

struct timers {
	int timerCounter;
	int dividerCounter;
};

//byte 2 (zero based) of TMC specifies whether the timer is enabled/disabled
void initialiseTimerCounter(struct gameboy * gameboy);
bool isTimerEnabled(struct gameboy * gameboy);
int getTimerFrequency(struct gameboy * gameboy);
void updateTimers(struct gameboy * gameboy, int cycles);

#endif
