#include <stdlib.h>
#include <stdio.h>
#include "gameboy.h"
#include "registers.h"

static void initialiseCPU(struct gameboy * gameboy);
static void initialiseMemory(struct gameboy * gameboy);

struct gameboy * createGameboy()
{
	struct gameboy * gameboy;
	gameboy = calloc(sizeof(struct gameboy), 0);
	
	if (gameboy == NULL){
		//handle this in calling function
		return NULL;
	}

	initialiseCPU(gameboy);
	initialiseMemory(gameboy);
	return gameboy;

	
	
}

static void initialiseCPU(struct gameboy * gameboy)
{
	gameboy->cpu.af = INIT_AF;
	gameboy->cpu.bc = INIT_BC;
	gameboy->cpu.de = INIT_DE;
	gameboy->cpu.hl = INIT_HL;
	gameboy->cpu.sp = INIT_STACK_POINTER;
	gameboy->cpu.pc = INIT_PROGRAM_COUNTER;

}

static void initialiseMemory(struct gameboy * gameboy)
{
	//initialise I/O stuff using writeMemory
	//directly alter memory if needs be (eg certain timers that reset if written 
	//to 'properly' through writeMemory)
}

void destroyGameboy(struct gameboy * gameboy)
{
	free(gameboy);
}
