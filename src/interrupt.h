#ifndef INTERRUPT_H
#define INTERRUPT_H

#define INTERRUPT_REQUEST_REG 0xFF0F
#define INTERRUPT_ENABLED_REG 0xFFFF

#include <stdbool.h>
#include <stdint.h>

#define NO_OF_INTERRUPTS 5

struct gameboy;

//corresponds to interrupt bits in flag registers
enum interrupt {
	vblank,
	lcdStat,
	timer,
	serial,
	joypad
};

struct interruptstruct {
	const char * name;
	void * function;
	uint8_t bit;
};

const struct interruptstruct interrupts[NO_OF_INTERRUPTS] = {
	{"V-Blank", vblankInterrupt, 0},
	{"LCD", lcdInterrupt, 1},
	{"Timer", timerInterrupt, 2},
	{"Serial", serialInterrupt, 3},
	{"Joypad", joypadInterrupt, 4}
};

struct interrupts {
	bool masterEnable;
	uint8_t intEnable;
	uint8_t intRequest;
};



//when an interrupt signal changes from low to high, then the corresponding bit in the 
//interrupt flag register becomes set. Eg Bit 0 becomes set when the LCD controller enters
//into the vblank period

//any bits set in the IF reg are only REQUESTING an interrupt to be executed. 
//The interrupt is executed if the Interrupt Master Enable Flag (IME) and the corresponding bit in the
//Interrupt Enable (IE) register are set. Otherwise, the interrupt waits until both IME and IE allow its execution

//when an interrupt is executed, the corresponding bit in the IF register is reset by the CPU, and the IME flag is cleared.
//The clearing of the IME flag disables any further interrupts until the program re-enableds the interrupts (usually by the RETI function) 
//and the corresponding interrupt vector (at 0040-0060)

//bit 0 in IE and IF registers have the highest priority, bit 4 the lowest

void requestInterrupt(struct gameboy * gameboy, enum interrupt);
void setInterruptMasterFlag(struct gameboy * gameboy, bool state);
void serviceInterrupts(struct gameboy * gameboy);


#endif
