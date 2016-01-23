#include "../include/memory.h"
#include "../include/gameboy.h"
#include "../include/dma.h"
#include "../include/joypad.h"
#include "../include/debug.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

static void handleBankWrite(struct gameboy * gameboy, uint16_t address, uint8_t data);
static void handleMBC1RAMBankToggle(struct gameboy * gameboy, uint8_t data);
static void handleMBC1LowROMBankNumber(struct gameboy * gameboy, uint8_t data);
static void handleMBC1HighROMBankNumber(struct gameboy * gameboy, uint8_t data);
static void handleMBC1ROMRAMModeSelect(struct gameboy * gameboy, uint8_t data);
static void handleRAMBankChange(struct gameboy * gameboy, uint8_t data);

void writeByte(struct gameboy * gameboy, uint16_t address, uint8_t data)
{
	//0000-8000 is read only
	if (address < CARTRIDGE_SIZE){
		handleBankWrite(gameboy, address, data);
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
		//printf("sp: %x\n", gameboy->cpu.sp);
		//printf("WriteMemory: address %x is within restricted memory %x - %x\n", address, RESTRICTED_START, RESTRICTED_END);
	//	fprintf(stderr, "WriteMemory: address %x is within restricted memory %x - %x\n", address, RESTRICTED_START, RESTRICTED_END);
		//printDebugTrace(gameboy);
		
	}
	else if (address == TMC){
		//the game is trying to change the timer controller
		int currentFreq = getTimerFrequency(gameboy);
		gameboy->memory.mem[TMC] = data;
		int newFreq = getTimerFrequency(gameboy);
		if (newFreq != currentFreq){
			initialiseTimerCounter(gameboy);
		}
	}
	else if (address == DIV_REG){
		//any writes to the divider register resets it to 0
		gameboy->memory.mem[DIV_REG] = 0;
	}
	else if (address == DMA_ADDRESS){
		doDMATransfer(gameboy, data);
	}
	else if (address == CONTROL_REG){
		gameboy->screen.control = data;
	}
	else if (address == CURRENT_SCANLINE){
		//printf("resetting scanline\n");
		gameboy->screen.currentScanline = 0;
	}
	else if (address == STATUS_REG){
		gameboy->screen.status = data;
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

static void handleBankWrite(struct gameboy * gameboy, uint16_t address, uint8_t data)
{
	//extend this into an array of function pointers - handleMBC1, handleMBC2... rather than have a big ifelse statement for each bank mode
	//get support for ROM only and MBC1 going first
	if (gameboy->cartridge.bankMode == ROM_ONLY){
		fprintf(stderr, "Cannot write to ROM\n");
	}
	else if (gameboy->cartridge.bankMode == MBC1 || gameboy->cartridge.bankMode == MBC1_RAM){
		if (address < RAM_BANK_ENABLE_UPPER){
			handleMBC1RAMBankToggle(gameboy, data);
		}
		else if ((address >= ROM_BANK_NUMBER_LOWER) && (address <= ROM_BANK_NUMBER_UPPER)){
			//select lower 5 bits of the ROM Bank Number
			handleMBC1LowROMBankNumber(gameboy, data);
		}
		else if ((address >= RAM_ROM_BANK_NUMBER_LOWER) && (address <= RAM_ROM_BANK_NUMBER_UPPER)){
			//select ram bank number or upper bits of ROM bank number
			if (gameboy->cartridge.romBanking){
				handleMBC1HighROMBankNumber(gameboy, data);
			}
			else {
				handleRAMBankChange(gameboy, data);
			}
		}
		else if ((address >= ROM_RAM_MODE_SELECT_LOWER) && (address <= ROM_RAM_MODE_SELECT_UPPER)){
			//select whether the two bits of the register above should be used as the upper two bits of the ROM bank, or as
			//a RAM Bank Number
			handleMBC1ROMRAMModeSelect(gameboy, data);
		}
	}
	else {
		fprintf(stderr, "MBC Mode not supported as of yet.\n");
	}

}

static void handleMBC1RAMBankToggle(struct gameboy * gameboy, uint8_t data)
{
	//check the lower 4 bits of the data
	//if they are == A, then enable RAM
	//else, disable RAM
	if ((data & 0xF) == 0xA){
		gameboy->cartridge.ramEnabled = true;
	}
	else {
		gameboy->cartridge.ramEnabled = false;
	}
}

static void handleMBC1LowROMBankNumber(struct gameboy * gameboy, uint8_t data)
{
	uint8_t lower5Bits = data & 0x1F;
	gameboy->cartridge.currentROMBank &= 0xE0; //turn off lower 5 bits
	gameboy->cartridge.currentROMBank |= lower5Bits; //set selected bits to 1
	//if this operation causes the currentROMBank to be 0 (ie 0x0-0x4000, which should never be loaded, treat it is ROM bank 1
	if (gameboy->cartridge.currentROMBank == 0){
		gameboy->cartridge.currentROMBank++;
	}
}

static void handleMBC1HighROMBankNumber(struct gameboy * gameboy, uint8_t data)
{
	//if romBanking is true, and when writing to 0x4000-0x6000, bits 5 and 6 of the ROM bank number are changed
	//turn off the upper 3 bits of the current ROM bank
	gameboy->cartridge.currentROMBank &= 0x1F;
	//turn off the lower 5 bits of data
	data &= 0xE0;
	//set bits
	gameboy->cartridge.currentROMBank |= data;
	//do the same check to see if ROM bank == 0
	if (gameboy->cartridge.currentROMBank == 0){
		gameboy->cartridge.currentROMBank++;
	}
}

static void handleRAMBankChange(struct gameboy * gameboy, uint8_t data)
{
	//RAM bank changes when writing to 0x4000-0x6000 but ROM banking is false
	gameboy->cartridge.currentRAMBank = data & 0x3; //currentRAMBank gets set to lower 2 bits of incoming data
}

static void handleMBC1ROMRAMModeSelect(struct gameboy * gameboy, uint8_t data)
{
	//tidy this up
	//turn ROM banking on/off
	uint8_t lsb = data & 0x1; //if the lsb of incoming data is 0 then romBanking is true, else it is false.
	//this means that there is about to be a RAM bank change.
	gameboy->cartridge.romBanking = (lsb == 0) ? true : false;
	if (gameboy->cartridge.romBanking){
		gameboy->cartridge.currentRAMBank = 0;
	}

}

uint8_t readByte(struct gameboy * gameboy, uint16_t address)
{
	//certain reads reset certain timers, implement this later
	//sort out ram/rom banks

	//memory range 0x4000 - 7FFF contain the ROM banks for each MBC type
	//do MBC reads here
	//don't need this if no MBC is present in the cart
	if ((address >= MBANK_START) && (address <= MBANK_END)){
		//if its in this range, get memory from cartridge
		return gameboy->cartridge.memory[(address - MBANK_START) + (gameboy->cartridge.currentROMBank * MBANK_START)];
	}
	else if ((address >= RAM_BANK_START) && (address <= RAM_BANK_END)){
		//for MBC1, A000-BFFF contains RAM Bank 00-03, if they exist.
		//for the time being, read from a RAM bank array.
		//if each bank is 0x2000 kb, and there is a max of 4 banks, then
		//the ram bank array is 0x8000 kb in size.
		return gameboy->cartridge.ramBanks[(address - RAM_BANK_START) + (gameboy->cartridge.currentRAMBank * RAM_BANK_SIZE)];
	}
	else if (address == CURRENT_SCANLINE){
		//printf("Reading scanline\n");
		return gameboy->screen.currentScanline;
	}
	else if (address == JOYPAD_REG){
		//look at data in memory address JOYPAD_REG (0xFF00)
		//see if the game is interested in directional or standard buttons
		//set joypad state as necessary
		return gameboy->memory.mem[address]; // try this
	}
	else if (address == CONTROL_REG){
		return gameboy->screen.control;
	}
	else if (address == STATUS_REG){
		return gameboy->screen.status;
	}

	return gameboy->memory.mem[address];

}

uint16_t readWord(struct gameboy * gameboy, uint16_t address)
{
	//get two consecutive bytes and merge them
	uint16_t result = readByte(gameboy, address) | (readByte(gameboy, address + 1) << 8);
	return result;
}


