#include "memory.h"
#include "gameboy.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void writeByte(struct gameboy * gameboy, uint16_t address, uint8_t data)
{
	//0000-8000 is read only
	if (address < CARTRIDGE_SIZE){
		fprintf(stderr, "Cannot write to memory between 0 and %x\n", CARTRIDGE_SIZE);
	}

	//anything written to echo RAM should be written to work RAM.
	else if ((address >= ECHO_RAM_START_UPPER) && (address < ECHO_RAM_END_UPPER)){
		gameboy->memory.mem[address] = data;
		gameboy->memory.mem[address - ECHO_OFFSET] = data;
	}

	else if ((address >= ECHO_RAM_START_LOWER) && (address < ECHO_RAM_END_LOWER)){
		gameboy->memory.mem[address] = data;
		gameboy->memory.mem[address + ECHO_OFFSET] = data;
	}

	else if ((address >= RESTRICTED_START) && (address < RESTRICTED_END)){
		fprintf(stderr, "WriteMemory: address %x is within restricted memory %x - %x\n", address, RESTRICTED_START, RESTRICTED_END);
	}

	else {
		gameboy->memory.mem[address] = data;
	}

}

void writeWord(struct gameboy * gameboy, uint16_t address, uint16_t data)
{
	writeByte(gameboy, address, data & 0x00FF);
	writeByte(gameboy, address + 1, (data & 0x00FF) >> 8);
}

uint8_t readByte(struct gameboy * gameboy, uint16_t address)
{
	//certain reads reset certain timers, implement this later
	//sort out ram/rom banks
	return gameboy->memory.mem[address];
}

uint16_t readWord(struct gameboy * gameboy, uint16_t address)
{
	//get two consecutive bytes and merge them
	uint16_t result = readByte(gameboy, address) | (readByte(gameboy, address + 1) << 8);
	return result;
}


