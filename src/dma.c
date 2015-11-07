#include "dma.h"
#include "gameboy.h"

void doDMATransfer(struct gameboy * gameboy, uint8_t data)
{
	uint16_t address = data << 8; //data * 100
	for (int i = SPRITE_RAM_START; i < 0xA0; i++){
		uint8_t byte = readByte(gameboy, address + i);
		writeByte(gameboy, i, byte);
	}
}
