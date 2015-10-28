#ifndef TIMER_H
#define TIMER_H

#include "gameboy.h"

/*
Timer is located at 0xFF05.
Counts up at a frequency set by the timer controller at 0xFF07
When it overflows (ie > 255) it fires a timer interrupt and resets to the value
at 0xFF06, in the timer modulator
*/

#define TIMA 0xFF05
#define TMA 0xFF06
#define TMC 0xFF07

#define TIMER_ENABLED_BIT 2

//byte 2 (zero based) of TMC specifies whether the timer is enabled/disabled
bool isTimerEnabled(struct gameboy * gameboy);

#endif
