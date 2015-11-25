#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "../include/gameboy.h"
#include "../include/registers.h"
#include "../include/joypad.h"
#include "../include/display.h"

static void initialiseCPU(struct gameboy * gameboy);
static void initialiseMemory(struct gameboy * gameboy);
static void initialiseControls(struct gameboy * gameboy);

struct gameboy * createGameboy()
{
	printf("Creating GameBoy structure... ");
	struct gameboy * gameboy;
	gameboy = malloc(sizeof(struct gameboy));
	
	if (gameboy == NULL){
		//handle this in calling function
		return NULL;
	}
	printf("done.\n");

	reset(gameboy);
	return gameboy;
	
}

void startEmulationLoop(struct gameboy * gameboy)
{
	//get instruction
	//execute instruction
	/*while(true){
		executeNextOpcode(gameboy);
		serviceInterrupts(gameboy);
		//sleep(1);
	}*/

	SDL_Event event;
	Uint8 * keys = NULL;
	bool quit = false;

	while(!quit){
		doJoypad(gameboy, &event, &keys, &quit);
		update(gameboy); //call this 60 times a second
	}

	SDL_Quit();

}

void update(struct gameboy * gameboy)
{
	int cycles = 0;
	while (cycles <= CYCLES_PER_FRAME){
		executeNextOpcode(gameboy);
		updateTimers(gameboy);
		updateGraphics(gameboy);
		serviceInterrupts(gameboy);
		sleep(1);
		cycles += gameboy->cpu.cycles;
		printf("%d\n", cycles);
	}

	renderGraphics(gameboy);
	gameboy->cpu.cycles = 0;
	printf("update done\n");
}

void reset(struct gameboy * gameboy)
{
	initialiseCPU(gameboy);
	initialiseMemory(gameboy);
	initialiseControls(gameboy);
}

static void initialiseCPU(struct gameboy * gameboy)
{
	printf("Resetting CPU... ");
	gameboy->cpu.af = INIT_AF;
	gameboy->cpu.bc = INIT_BC;
	gameboy->cpu.de = INIT_DE;
	gameboy->cpu.hl = INIT_HL;
	gameboy->cpu.sp = INIT_STACK_POINTER;
	gameboy->cpu.pc = INIT_PROGRAM_COUNTER;
	printf("done.\n");

}

static void initialiseMemory(struct gameboy * gameboy)
{
	//initialise I/O stuff using writeMemory
	//directly alter memory if needs be (eg certain timers that reset if written 
	//to 'properly' through writeMemory)
	printf("Resetting memory... ");
	memset(gameboy->memory.mem, 0, sizeof(gameboy->memory.mem));
	memset(gameboy->cartridge.memory, 0, sizeof(gameboy->cartridge.memory));
	
	gameboy->interrupts.masterEnable = true;

	gameboy->memory.mem[0xFF05] = 0x0; //TIMA
	gameboy->memory.mem[0xFF06] = 0x0;
	gameboy->memory.mem[0xFF07] = 0x0;
	gameboy->memory.mem[0xFF10] = 0x80;
	gameboy->memory.mem[0xFF11] = 0x80;
	gameboy->memory.mem[0xFF12] = 0xF3;
	gameboy->memory.mem[0xFF14] = 0xBF;
	gameboy->memory.mem[0xFF16] = 0x3F;
	gameboy->memory.mem[0xFF17] = 0x0;
	gameboy->memory.mem[0xFF19] = 0xBF;
	gameboy->memory.mem[0xFF1A] = 0x7F;
	gameboy->memory.mem[0xFF1B] = 0xFF;
	gameboy->memory.mem[0xFF1C] = 0x9F;
	gameboy->memory.mem[0xFF1E] = 0xBF;
	gameboy->memory.mem[0xFF20] = 0xFF;
	gameboy->memory.mem[0xFF21] = 0x00;
	gameboy->memory.mem[0xFF22] = 0x00;
	gameboy->memory.mem[0xFF23] = 0xBF;
	gameboy->memory.mem[0xFF24] = 0x77;
	gameboy->memory.mem[0xFF25] = 0xF3;
	gameboy->memory.mem[0xFF26] = 0xF1;
	gameboy->memory.mem[0xFF40] = 0x91;
	gameboy->memory.mem[0xFF42] = 0x0;
	gameboy->memory.mem[0xFF43] = 0x0;
	gameboy->memory.mem[0xFF45] = 0x0;
	gameboy->memory.mem[0xFF47] = 0xFC;
	gameboy->memory.mem[0xFF48] = 0xFF;
	gameboy->memory.mem[0xFF49] = 0xFF;
	gameboy->memory.mem[0xFF4A] = 0x0;
	gameboy->memory.mem[0xFF4B] = 0x0;
	gameboy->memory.mem[0xFFFF] = 0x0;

	printf("done\n");

}

static void initialiseControls(struct gameboy * gameboy)
{
	gameboy->joypad.state = malloc(sizeof(int) * NO_OF_BUTTONS); //calloc with 1?
	gameboy->joypad.previousState = malloc(sizeof(int) * NO_OF_BUTTONS); //calloc with 1?
	gameboy->memory.mem[JOYPAD_REG] = JOYPAD_REG_INIT; //00001111
	gameboy->joypad.buttonState = 0xFF; //all bits set to 1
}

void destroyGameboy(struct gameboy * gameboy)
{
	free(gameboy);
}
