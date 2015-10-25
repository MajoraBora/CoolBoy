#ifndef MEMORY_H 
#define MEMORY_H 

#include <stdint.h>
#include <stdbool.h>
#include "cartridge.h"

#define TOTAL_MEMORY_SIZE 0xFFFF
#define MAX_CART_SIZE 0x20000
#define CARTRIDGE_SIZE 0x8000
#define VRAM_SIZE 0x2000
#define EXTERNAL_RAM_SIZE 0x2000
#define WORK_RAM_SIZE 0x2000
#define SPRITE_TABLE_SIZE 0x100
#define IO_SIZE 0x100
#define HIGH_RAM_SIZE 0x80

#define ECHO_RAM_START_UPPER 0xE000
#define ECHO_RAM_END_UPPER 0xFE00
#define ECHO_RAM_START_LOWER 0xC000
#define ECHO_RAM_END_LOWER 0xDE00
#define ECHO_OFFSET 0x2000

#define EX_RAM_START 0xA000
#define EX_RAM_END 0xBFFF

#define RESTRICTED_START 0xFEA0
#define RESTRICTED_END 0xFEFF

/*
struct memory {
	//0000-3FFF - 16kb ROM BANK 00
	//4000-7FFF - 16kb ROM Bank 01..NN
	//This area is typically used to address ROM. Cartridges with MBCs (Memory
	//Bank Controllers) use this area to output data (write only) to the MBC chip.
	//0000-8000 is read only
	unsigned char cartridge[CARTRIDGE_SIZE];
	//8000-9FFF - 8kb Video RAM
	unsigned char videoRam[VRAM_SIZE];
	//A000-BFFF - 8kb External RAM
	//Used to address external RAM or external hardware. May hold saved game 
	//positions and high score tables, even when the gameboy is turned off.
	unsigned char externalRam[EXTERNAL_RAM_SIZE];
	//C000-CFFF - 4kb Work RAM Bank 0
	//D000-DFFF - 4kb Work RAM Bank 1
	unsigned char workRam[WORK_RAM_SIZE];
	//FE00-FE9F - Sprite Attribute Table (OAM)
	unsigned char spriteTable[SPRITE_TABLE_SIZE];
	//FF00-FF7F - I/O Ports
	unsigned char io[IO_SIZE];
	//FF80 - FFFE - High Ram (HRAM)
	unsigned char highRam[HIGH_RAM_SIZE];
};
*/

struct gameboy; //forward declaration
/*
enum mbcMode {
	ROM_ONLY = 0x0,
	MBC1 = 0x01, 
	MBC1_RAM = 0x02,
	MBC_RAM_BATTERY = 0x03,
	MBC2 = 0x05,
	MBC2_BATTERY = 0x06,	
	ROM_RAM = 0x08, 
	ROM_RAM_BATTERY = 0x09,
	MMM01 = 0x0B,
	MMM01_RAM = 0x0C, 
	MMM01_RAM_BATTERY = 0x0D, 
	MBC3_TIMER_RAM_BATTERY = 0x0F,
	MBC3 = 0x11, 
	MBC3_RAM = 0x12, 
	MBC3_RAM_BATTERY = 0x13, 
	MBC4 = 0x15, 
	MBC4_RAM = 0x16, 
	MBC4_RAM_BATTERY = 0x17, 
	MBC5 = 0x19,
	MBC5_RAM = 0x1A, 
	MBC5_RAM_BATTERY = 0x1B, 
	MBC5_RUMBLE = 0x1C, 
	MBC5_RUMBLE_RAM = 0x1D, 
	MBC5_RUMBLE_RAM_BATTERY = 0x1E, 
	POCKET_CAMERA = 0xFC, 
	BANDAIT_TAMA5 = 0xFD, 
	HUC3 = 0xFE,
	HUC1_RAM_BATTERY = 0xFF
};
*/


struct memory {
	uint8_t mem[TOTAL_MEMORY_SIZE];
	uint8_t currentBank;
	bool externalRamEnabled;
};

void writeByte(struct gameboy * gameboy, uint16_t address, uint8_t data);
void writeWord(struct gameboy * gameboy, uint16_t address, uint16_t data);
uint8_t readByte(struct gameboy * gameboy, uint16_t address);
uint16_t readWord(struct gameboy * gameboy, uint16_t address);

#endif
