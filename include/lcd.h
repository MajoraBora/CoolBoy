#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdbool.h>

/*
The gameboy draws each scanline from 0 to 153. 
From 144 to 153 is the v blank period.
Each scanline takes 456 cpu clock cycles to draw. This can be kept track of 
with a counter, similar to how the timers work.
*/

#define X 160
#define Y 144

#define NO_OF_INVISIBLE_SCANLINES 9

#define CONTROL_REG 0xFF40
#define STATUS_REG 0xFF41
#define SCROLL_X 0xFF42
#define SCROLL_Y 0xFF43
#define CURRENT_SCANLINE 0xFF44
//when LY_COMPARE == coincident bit in status reg, stat interrupt is requested
#define LY_COMPARE 0xFF45

//Specifes the upper/left positions of the window - an alternate background
//area which is displayed above the normal background.
//the window is visible, if enabled, when positions are in the range
	//WX= 0 to 166
	//WY = 0 to 143
//A position of WX=7, WY=0 locates the window at the upper left, completely
//covering the background

#define WINDOW_POS_X 0xFF4A
#define WINDOW_POS_Y 0xFF4B

#define BG_PALETTE_DATA 0xFF47
#define OBJECT_PALETTE_0_DATA 0xFF48
#define OBJECT_PALETTE_1_DATA 0xFF49

#define SCANLINE_CYCLE_TIME 456

#define SPRITE_ATTR_CYCLE_LOWER 0
#define SPRITE_ATTR_CYCLE_UPPER 80
#define DRIVER_TRANSFER_CYCLE_LOWER 80
#define DRIVER_TRANSFER_CYCLE_UPPER 252

#define HBLANK_ENABLE_BIT 3
#define VBLANK_ENABLE_BIT 4
#define SPRITE_ENABLE_BIT 5
#define COINCIDENCE_BIT 2
#define COINCIDENCE_ENABLE_BIT 6

/*
The background is made up of 256x256 pizels (32x32) tiles
However, we only display 160x144 pixels, so there is no need to draw the rest.
We need to know where to draw the background tiles - use scrollY, scrollX, windowY and windowX
 	scroll - The positions of the background
 	window - the positions of the viewing area
To draw the window in the upper left hand corner of the viewing area:
	windowX = -7
	windowY = 0
If the window was drawn from the top left corner, then it would copmletely cover the background
If the window is only to cover the bottom half of the background:
	windowX = 7
	windowY = (144/2)

The gameboy has two regions of memory containing what the gameboy needs to draw, 0x9800-0x9BFF and 0x9C00-0x9FFF
Bit 3 of the LCD control register controls which area to use for the background.
Bit 6 controls which area to use for the window.

Each byte in this memory region is a tile ID number of what needs to be drawn. This is used to lookup the tile
data in VRAM.

Bit 4 of the control register selects between two memory regions that contain tile data to draw.
	0x8800-0x97FF (0)
	0x8000-0x8FFF (1)

Each tile is 16 bytes - 8x8 pixels in size, 2 bytes per horizontal line of the tile.
If the tile memory region is (0), the tile ID from the background regions is read as a signed byte. 
If it is (1), the tile ID is read as an unsigned byte. 

Drawing a tile/sprite:

The background layout region identifies each tile in the current background that needs to be drawn.
The tile ID from this background region is used to lookup tile data in the tile data region.
If two bytes of data form one line of the tile, then they need to be combined for form a break down of each pixel in the 8 pixel line
Example:
	pixel# - 1 2 3 4 5 6 7 8
	byte 1 - 0 0 1 1 0 1 0 1
	byte 2 - 1 0 1 0 1 1 1 0

	p1 - 10
	p2 - 00
	p3 - 11
	p4 - 01
	p5 - 10
	p6 - 11
	p7 - 10
	p8 - 01

Palettes are used to map these colour IDs to the correct colours. 
Palettes are not fixed - the colour of tiles and sprites can be changed without changing the tile data.

Background tiles have 1 palette stored at 0xFF47. Sprites have 2 at 0xFF48/49.
Every 2 bits in a palette represent a colour
	00 - white - bits 1/0
	01 - light grey - bits 3/2
	10 - dark grey - bits 5/4
	11 - black - bits 7/6
*/

#define REGION_TWO_BG_MEMORY 0x9C00
#define REGION_ONE_BG_MEMORY 0x9800

struct gameboy;
struct colour {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct screen {
	uint8_t control;
	uint8_t status;
	uint8_t scrollY;
	uint8_t scrollX;
	uint8_t currentScanline; //writing resets the counter
	int scanlineCounter;
	//redirect read from 0xFF44 to currentScanline
	uint8_t windowXPos;
	uint8_t windowYPos;
	bool currentLCDInterruptEnabled;
	struct colour frameBuffer[X * Y];
};

enum controlBit {
	bgDisplayEnable,
	spriteEnable,
	spriteSize,
	bgTileDisplaySelect,
	bgWindowTileDataSelect,
	windowDisplayEnable,
	windowTileMapDisplaySelect,
	lcdEnable
};

enum statusBitMode {
	hBlank,
	vBlank,
	oamRead,
	oamVramRead
};


/*
The memory address at 0xFF41 holds the current status of the LCD. The LCD goes through
4 different modes: vBlank period, hBlank period, searching for sprite attributes, and 
transferring data to LCD driver. 

Bit 1 and 0 of the lcd status value contains the current LCD mode.

When the LCD status changes its mode to either mode 0, 1 or 2, an interrupt request
occurs. Bits 3, 4, and 5 of the status register are interrupt enabled flags, like the 
one at 0xFFFF for the other interrupts.
Bit 3 - Mode 0 enabled (hblank)
bit 4 - Mode 1 enabled (vblank)
bit 5 - Mode 2 enabled (oam read)

When the mode changes to 0, 1 or 2, and if the the corresponding bit at 3, 4 or 5 is 
set, then an LCD interrupt is requested.

When the LCD is disabled, its mode must be set to 1

Bit 2 of the status register is set to 1 if register 0xFF44 is the same value as
0xFF45. Otherwise, it is set to 0. Bit 6 of the status register is the like the 
previous "enabled" flags, only for bit 2. In other words, if the coincidence flag is
set, then the current scanline (0xFF44) is the same as a scanline the game is 
interested in (0xFF45). This is usually used to do special effects on the display.

If they are the same values, then an interrupt is requested
*/
void setLCDStatus(struct gameboy * gameboy);
bool isLCDEnabled(struct gameboy * gameboy);
void updateGraphics(struct gameboy * gameboy);
void updateGraphicsTest(struct gameboy * gameboy);
void drawScanline(struct gameboy * gameboy);
#endif
