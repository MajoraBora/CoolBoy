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

int main(void)
{
	
	struct gameboy * gameboy;
	gameboy = createGameboy();

	//startEmulationLoop(gameboy);
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	printf("%d, %d\n", gameboy->cpu.hl, byte);
	cb_rlc_hlp(gameboy);
	

	return 0;


}

