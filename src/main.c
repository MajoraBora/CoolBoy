#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cpu.h"
#include "registers.h"
#include "memory.h"
#include "gameboy.h"
#include "cartridge.h"
#include "timer.h"
#include "bitUtils.h"

int main(void)
{
	
	struct gameboy * gameboy;
	gameboy = createGameboy();

	loadGame(gameboy, "tetris.gb");

	startEmulationLoop(gameboy);
	
	destroyGameboy(gameboy);

	return 0;


}

