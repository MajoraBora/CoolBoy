#include <stdio.h>
#include <stdlib.h>
#include "../src/flags.h"
#include "../src/gameboy.h"

int main(void)
{
	struct gameboy * gameboy = malloc(sizeof(struct gameboy));
	gameboy->cpu.f = 0x0;
	bool result = isFlagSet(gameboy, ZERO);
	printf("%d\n", result);

	printf("%x\n", gameboy->cpu.f);
	setFlag(gameboy, ZERO, true);
	printf("%x\n", gameboy->cpu.f);
	setFlag(gameboy, SUB, true);
	printf("%x\n", gameboy->cpu.f);
	setFlag(gameboy, SUB, false);
	printf("%x\n", gameboy->cpu.f);
	
	return 0;
}
