#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/cpu.h"
#include "../include/registers.h"
#include "../include/memory.h"
#include "../include/gameboy.h"
#include "../include/cartridge.h"
#include "../include/timer.h"
#include "../include/bitUtils.h"
#include "../include/extops.h"
#include "../include/interrupt.h"
#include "../include/joypad.h"
#include "../include/display.h"

int main(void)
{
	
	struct gameboy * gameboy;
	gameboy = createGameboy();
	loadGame(gameboy, "../games/tetris.gb");
	startDisplay();

	startEmulationLoop(gameboy);
	


	return 0;


}

