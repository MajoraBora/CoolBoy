#include "interrupt.h"
#include "gameboy.h"
#include "bitUtils.h"
#include <stdio.h>

const uint8_t interruptRoutineAddresses[NO_OF_INTERRUPTS] = {
	0x40, //vblank
	0x48, //lcd
	0x50, //timer
	0x58, //serial
	0x60 //joypad
};

static void doInterrupt(struct gameboy * gameboy, int i);

void requestInterrupt(struct gameboy * gameboy, enum interrupt interrupt)
{
	//called when an event needs to trigger an interrupt.
	//sets the corresponding bit in the Interrupt Request Register(0xFF0F)
	//stored at gameboy->interrupts...

	printf("Requesting interrupt %d\n", interrupt);
	printf("%d\n", gameboy->interrupts.intRequest);
	setBit(&gameboy->interrupts.intRequest, interrupt, true);
	printf("%d\n", gameboy->interrupts.intRequest);
}

void setInterruptMasterFlag(struct gameboy * gameboy, bool state)
{
	gameboy->interrupts.masterEnable = state;
}

void serviceInterrupts(struct gameboy * gameboy)
{
	//if masterEnable is enabled
	//get all bits that have been enabled in intEnable
	//go through each enabled bit in intRequest, starting from highest priority (0)
	
	if (gameboy->interrupts.masterEnable){
		uint8_t enabledRequests = gameboy->interrupts.intRequest & gameboy->interrupts.intEnable;
		//If the bit is set, do the interrupt
		for (int i = vblank; i <= joypad; i++){
			printf("%d: %d\n", i, enabledRequests);
			if (isBitSet(enabledRequests, i)){
				//interruptFunctions[i]; //execute correct function
				printf("doing interrupt %d\n", i);
				doInterrupt(gameboy, i);
			}
		}
	}
}

static void doInterrupt(struct gameboy * gameboy, int i)
{
	gameboy->interrupts.masterEnable = false;
	setBit(&gameboy->interrupts.intRequest, i, false); // reset bit
	gameboy->cpu.pc = interruptRoutineAddresses[i]; 
}

