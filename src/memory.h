#ifndef MEMORY_H 
#define MEMORY_H 


#define TOTAL_MEMORY_SIZE 0xFFFF
#define MAX_CART_SIZE 0x20000
#define CARTRIDGE_SIZE 0x8000
#define VRAM_SIZE 0x2000
#define EXTERNAL_RAM_SIZE 0x2000
#define WORK_RAM_SIZE 0x2000
#define SPRITE_TABLE_SIZE 0x100
#define IO_SIZE 0x100
#define HIGH_RAM_SIZE 0x80

#define ECHO_RAM_START 0xE000
#define ECHO_RAM_END 0xFE00
#define ECHO_OFFSET 0x2000

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

struct memory {
	uint8_t mem[TOTAL_MEMORY_SIZE];
	uint8_t cart[MAX_CART_SIZE];
};

void writeMemory(struct gameboy * gameboy, uint16_t address, uint8_t data);
void readMemory(struct gameboy * gameboy, uint16_t address);

#endif
