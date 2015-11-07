#include "lcd.h"
#include "bitUtils.h"
#include "gameboy.h"
#include <stdio.h>

static void doScanline(struct gameboy * gameboy);
static void handleCurrentScanline(struct gameboy * gameboy);
static void startNewScanline(struct gameboy * gameboy);
static uint8_t getCurrentMode(struct gameboy * gameboy);

void updateGraphics(struct gameboy * gameboy, int cycles)
{
	setLCDStatus(gameboy);
	if (isLCDEnabled(gameboy)){
		//handleVBlank(gameboy);
		//work out what to do here
		gameboy->screen.scanlineCounter += cycles;
		doScanline(gameboy);
	}
}

static void doScanline(struct gameboy * gameboy)
{
	if (gameboy->screen.scanlineCounter >= SCANLINE_CYCLE_TIME){
		//if the number of cycles for this scanline is over 456...
		//start moving onto the next scan line
		gameboy->screen.currentScanline++;
		handleCurrentScanline(gameboy);
	}
}

static void handleCurrentScanline(struct gameboy * gameboy)
{
	uint8_t currentScanline = gameboy->screen.currentScanline;
	if (currentScanline == Y){
		requestInterrupt(gameboy, vblank);
	}
	else if (currentScanline > (Y + NO_OF_INVISIBLE_SCANLINES)){
		gameboy->screen.currentScanline = 0; //reset
	}
	else {
		drawScanline(gameboy);
	}
}

bool isLCDEnabled(struct gameboy * gameboy)
{
	//refactor -replace this with a bool field in struct, lcdEnabled or something
	return isBitSet(gameboy->screen.control, lcdEnable);
}

void setLCDStatus(struct gameboy * gameboy)
{
	if (!isLCDEnabled(gameboy)){
		startNewScanline(gameboy);
	}
	else {
		uint8_t currentMode = getCurrentMode(gameboy);
		printf("%d\n", currentMode);
		
	}
}

void drawScanline(struct gameboy * gameboy)
{

}

static uint8_t getCurrentMode(struct gameboy * gameboy)
{
	return gameboy->screen.status & 0x3;
}

static void startNewScanline(struct gameboy * gameboy)
{
	//if LCD is disabled, set mode to 1
	gameboy->screen.scanlineCounter = 0;
	gameboy->screen.currentScanline = 0;
	gameboy->screen.status &= 252; //whats going on?
	setBit(&gameboy->screen.status, hBlank, true);
}
