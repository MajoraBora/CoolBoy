#include "interrupt.h"
#include "gameboy.h"
#include "bitUtils.h"

const struct interruptstruct interrupts[NO_OF_INTERRUPTS] = {
        {"V-Blank", vblankInterrupt, 0},
        {"LCD", lcdInterrupt, 1},
        {"Timer", timerInterrupt, 2},
        {"Serial", serialInterrupt, 3},
        {"Joypad", joypadInterrupt, 4}
};

static void setInterruptRequest(struct gameboy * gameboy, enum interrupt interrupt);

void requestInterrupt(struct gameboy * gameboy, enum interrupt interrupt)
{
	//called when an event needs to trigger an interrupt.
	//sets the corresponding bit in the Interrupt Request Register(0xFF0F)
	//stored at gameboy->interrupts...

	setInterruptRequest(gameboy, interrupt);
}

static void setInterruptRequest(struct gameboy * gameboy, enum interrupt interrupt)
{
	setBit(&gameboy->interrupts.intRequest, interrupt, true);
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
		for (int i = vblank; i < joypad; i++){
			if (isBitSet(enabledRequests, i)){
				//interruptFunctions[i]; //execute correct function
			}
		}
	}
}

