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

#include "../include/interrupt.h"

int main(void)
{
	
	struct gameboy * gameboy;
	gameboy = createGameboy();

	loadGame(gameboy, "../games/tetris.gb");

	//startEmulationLoop(gameboy);

	gameboy->cpu.a = 1;
	gameboy->cpu.f = 0;
	for (int i = 0; i < 10; i++){
		printf("%d, %d\n", gameboy->cpu.a, gameboy->cpu.f);
		rl_a(gameboy);
	}
	return 0;


}

