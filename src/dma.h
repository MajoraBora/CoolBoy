#ifndef DMA_H
#define DMA

/*
The CPU can only access the Sprite Attributes Table during LCD mode 2 (Sprite Attribute Search)
Direct Memory Access (DMA) is used to copy data to the sprite RAM at the appropriate time.

DMA is launched when the game attempts to write to memory address 0xFF46.

The destination address of the DMA is the sprite RAM between 0xFE00 - 0xFE9F.
This means that a total of 0xA0 bytes will be copied to this region.
The source address is the data being written to 0xFF46 multiplied by 100.
*/

#include <stdint.h>

#define DMA_ADDRESS 0xFF46

struct gameboy;

void doDMATransfer(struct gameboy * gameboy, uint8_t data);


#endif
