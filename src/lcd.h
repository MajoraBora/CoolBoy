#ifndef LCD_H
#define LCD_H

#include <stdint.h>

/*
The gameboy draws each scanline from 0 to 153. 
From 144 to 153 is the v blank period.
Each scanline takes 456 cpu clock cycles to draw. This can be kept track of 
with a counter, similar to how the timers work.
*/

#define X 160
#define Y 144

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

struct gameboy;

struct screen {
	uint8_t control;
	uint8_t status;
	uint8_t scrollY;
	uint8_t scrollX;
	uint8_t currentScanline; //writing resets the counter
	uint8_t windowXPos;
	uint8_t windowYPos;
};

void updateGraphics(struct gameboy * gameboy, int cycles);

#endif
