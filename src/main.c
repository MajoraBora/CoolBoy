#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cpu.h"
#include "registers.h"
#include "memory.h"
#include "gameboy.h"
#include "cartridge.h"

int main(void)
{
	
	struct gameboy * gameboy;
	gameboy = createGameboy();

	loadGame(gameboy, "sml.gb");

	startEmulationLoop(gameboy);
	
	destroyGameboy(gameboy);

	return 0;
}

