#ifndef GAMEBOY_H
#define GAMEBOY_H

#include "cpu.h"
#include "memory.h"
#include "timer.h"
#include "interrupt.h"
#include "lcd.h"
#include "joypad.h"

struct gameboy {
	struct cpu cpu;
	struct memory memory;
	struct cartridge cartridge;
	struct timers timers;
	struct interrupts interrupts;
	struct screen screen;
	struct joypad joypad;
	//have an error code field - if an error occurs, set it, exit the emu loop, 
	//then let the calling scope extract and handle it
	//struct error error;
};

struct gameboy * createGameboy();
void startEmulationLoop(struct gameboy * gameboy);
void reset(struct gameboy * gameboy);
void destroyGameboy(struct gameboy * gameboy);

#endif
