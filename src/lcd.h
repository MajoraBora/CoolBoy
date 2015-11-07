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
struct gameboy;

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
};

enum controlBit {
	bgDisplay,
	objDisplayEnable,
	objSize,
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
void updateGraphics(struct gameboy * gameboy, int cycles);
void drawScanline(struct gameboy * gameboy);
#endif
