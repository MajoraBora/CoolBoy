#ifndef GAMEBOY_H
#define GAMEBOY_H

#include "cpu.h"
#include "memory.h"

struct gameboy {
	struct cpu cpu;
	struct memory memory;
};

struct gameboy * createGameboy();
void destroyGameboy(struct gameboy * gameboy);

#endif
