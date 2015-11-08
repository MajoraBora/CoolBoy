#include "../include/stack.h"
#include <stdio.h>

void pushWordToStack(struct gameboy * gameboy, uint16_t word)
{
	gameboy->cpu.sp -= 2;
	writeWord(gameboy, gameboy->cpu.sp, word);
	printf("Pushing %x to stack\n", word);
}

uint16_t popWordFromStack(struct gameboy * gameboy)
{
	uint16_t word = readWord(gameboy, gameboy->cpu.sp);
	gameboy->cpu.sp += 2;
	return word;
}
