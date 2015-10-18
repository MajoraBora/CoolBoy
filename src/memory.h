#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#define CARTRIDGE_SIZE 0x8000
#define VRAM_SIZE 0x2000
#define EXTERNAL_RAM_SIZE 0x2000
#define WORK_RAM_SIZE 0x2000
#define SPRITE_TABLE_SIZE 0x100
#define IO_SIZE 0x100
#define HIGH_RAM_SIZE 0x80

struct memory {
	//0000-3FFF - 16kb ROM BANK 00
	//4000-7FFF - 16kb ROM Bank 01..NN
	//This area is typically used to address ROM. Cartridges with MBCs (Memory
	//Bank Controllers) use this area to output data (write only) to the MBC chip.
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

void writeMemory(struct gameboy * gameboy, uint16_t address, uint8_t data);
void readMemory(struct gameboy * gameboy, uint16_t address);

#endif
