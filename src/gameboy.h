#ifndef GAMEBOY_H
#define GAMEBOY_H

#include "cpu.h"
#include "memory.h"

struct gameboy {
	struct cpu cpu;
	struct memory memory;
	struct cartridge cartridge;
	//have an error code field - if an error occurs, set it, exit the emu loop, 
	//then let the calling scope extract and handle it
	//struct error error;
};

struct gameboy * createGameboy();
void startEmulationLoop(struct gameboy * gameboy);
void reset(struct gameboy * gameboy);
void destroyGameboy(struct gameboy * gameboy);

#endif
