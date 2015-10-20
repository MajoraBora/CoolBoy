#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cpu.h"
#include "registers.h"
#include "memory.h"
#include "gameboy.h"
#include "cartridge.h"

static void alterGameboy(struct gameboy * gameboy);

int main(void)
{
	
	struct gameboy * gameboy;
	gameboy = createGameboy();

	printf("%x\n", gameboy->cpu.af);
	printf("%x\n", gameboy->cpu.bc);
	printf("%x\n", gameboy->cpu.de);
	printf("%x\n", gameboy->cpu.hl);
	printf("%x\n", gameboy->cpu.sp);
	printf("%x\n", gameboy->cpu.pc);
	alterGameboy(gameboy);
	printf("%x\n", gameboy->cpu.af);

	loadGame(gameboy, "sml.gb");
	
	destroyGameboy(gameboy);

	return 0;
}

static void alterGameboy(struct gameboy * gameboy)
{
	gameboy->cpu.af++;
}
