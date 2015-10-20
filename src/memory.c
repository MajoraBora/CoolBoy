#include "gameboy.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "gameboy.h"
#include "memory.h"

void writeMemory(struct gameboy * gameboy, uint16_t address, uint8_t data)
{
	//0000-8000 is read only
	if (address < CARTRIDGE_SIZE){
		fprintf(stderr, "Cannot write to memory between 0 and %x\n", CARTRIDGE_SIZE);
	}

	//anything written to echo RAM should be written to work RAM.
	else if ((address >= ECHO_RAM_START) && (address < ECHO_RAM_END)){
		gameboy->memory.mem[address] = data;
		writeMemory(gameboy, address - ECHO_OFFSET, data);
	}

	else if ((address >= RESTRICTED_START) && (address < RESTRICTED_END)){
		fprintf(stderr, "WriteMemory: address %x is within restricted memory %x - %x\n", address, RESTRICTED_START, RESTRICTED_END);
	}

	else {
		gameboy->memory.mem[address] = data;
	}

}

void readMemory(struct gameboy * gameboy, uint16_t address)
{

}

