#include "../include/lcd.h"
#include "../include/bitUtils.h"
#include "../include/gameboy.h"
#include "../include/memory.h"
#include <stdio.h>

static void doScanline(struct gameboy * gameboy);
static void handleCurrentScanline(struct gameboy * gameboy);
static void startNewScanline(struct gameboy * gameboy);
static uint8_t getCurrentMode(struct gameboy * gameboy);

//static bool isInHBlankBounds(struct gameboy * gameboy);
static bool isInVBlankBounds(struct gameboy * gameboy);
static bool isInSpriteAttributeBounds(struct gameboy * gameboy);
static bool isInDriverTransferBounds(struct gameboy * gameboy);

static void handleHBlank(struct gameboy * gameboy);
static void handleVBlank(struct gameboy * gameboy);
static void handleSpriteSearch(struct gameboy * gameboy);
static void handleTransferToLCDDriver(struct gameboy * gameboy);
static void requestAnyNewModeInterrupts(struct gameboy * gameboy, uint8_t previousMode);
static void doCoincidenceFlag(struct gameboy * gameboy);

static bool backgroundTilesEnabled(struct gameboy * gameboy);
static bool spritesEnabled(struct gameboy * gameboy);

static void renderTiles(struct gameboy * gameboy);
static void renderSprites(struct gameboy * gameboy);
static bool windowEnabled(struct gameboy * gameboy);
static void renderTilesOnWindow(struct gameboy * gameboy);
static void renderTilesOnBackground(struct gameboy * gameboy);
static void (*getTileDataGetFunction(struct gameboy * gameboy))(struct gameboy * gameboy);
static void getTileDataFromRegion1(struct gameboy * gameboy);
static void getTileDataFromRegion2(struct gameboy * gameboy);

const void (*scanModeFuncs[4]) = {
	handleHBlank,
	handleVBlank,
	handleSpriteSearch,
	handleTransferToLCDDriver
};

const void (*tileDataFuncs[2]) = {
	getTileDataFromRegion1,
	getTileDataFromRegion2
};

void updateGraphics(struct gameboy * gameboy, int cycles)
{
	uint8_t previousMode = getCurrentMode(gameboy);
	setLCDStatus(gameboy);
	requestAnyNewModeInterrupts(gameboy, previousMode);
	doCoincidenceFlag(gameboy);
	if (isLCDEnabled(gameboy)){
		gameboy->screen.scanlineCounter += cycles;
		doScanline(gameboy);
	}
}

static void requestAnyNewModeInterrupts(struct gameboy * gameboy, uint8_t previousMode)
{
	bool enabled = gameboy->screen.currentLCDInterruptEnabled;
	uint8_t currentMode = getCurrentMode(gameboy);
	if (enabled && (currentMode != previousMode)){
		requestInterrupt(gameboy, lcdStat);
	}
	
}

static void doCoincidenceFlag(struct gameboy * gameboy)
{
	if (gameboy->screen.currentScanline == readByte(gameboy, LY_COMPARE)){
		//current scanline == the values stored at 0xFF45
		//if this is true, set bit 2 of the status reg. Otherwise, reset it.
		setBit(&gameboy->screen.status, COINCIDENCE_BIT, true);
		if (isBitSet(gameboy->screen.status, COINCIDENCE_ENABLE_BIT)){
			requestInterrupt(gameboy, lcdStat);
		}
	}
	else {
		setBit(&gameboy->screen.status, COINCIDENCE_BIT, false);
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
	//it take 456 clock cycles to draw 1 scanline
	//LCD status is in mode 2 for the first 80 (search sprite attrs)
	//Mode 3 is set for the next 172 (transfer to LCD driver)
	//The remaining 204 are for mode 0 (hBlank)

	//When the status mode is changed to 0, 1 or 2, an interrupt request occurs
	//Check bit 3, 4, or 5 to see if mode interrupt 0, 1 or 2 is enabled
	//when the lcd mode is disabled, the status mode must be set to 1
	if (!isLCDEnabled(gameboy)){
		//starting a new scan line sets the lcd status to 2
		startNewScanline(gameboy);
	}
	else {
		if (isInVBlankBounds(gameboy)){
			handleVBlank(gameboy);
		}
		else if (isInSpriteAttributeBounds(gameboy)){
			handleSpriteSearch(gameboy);
		}
		else if (isInDriverTransferBounds(gameboy)){
			handleTransferToLCDDriver(gameboy);
		}
		else {
			handleHBlank(gameboy);
		}
		
	}
}

void drawScanline(struct gameboy * gameboy)
{
	if (backgroundTilesEnabled(gameboy)){
		renderTiles(gameboy);
	}
	
	if (spritesEnabled(gameboy)){
		renderSprites(gameboy);
	}
}

static bool backgroundTilesEnabled(struct gameboy * gameboy)
{
	return isBitSet(gameboy->screen.control, bgDisplayEnable);
}

static bool spritesEnabled(struct gameboy * gameboy)
{
	return isBitSet(gameboy->screen.control, spriteEnable);
}

static void renderTiles(struct gameboy * gameboy)
{
	getTileDataGetFunction(gameboy);
	if(windowEnabled(gameboy)){
		if (gameboy->screen.windowYPos <= gameboy->screen.currentScanline){
			renderTilesOnWindow(gameboy);
		}
		else {
			renderTilesOnBackground(gameboy);
		}
	}
}

static void renderTilesOnWindow(struct gameboy * gameboy)
{
	uint16_t windowMemory = 0;
	if (isBitSet(gameboy->screen.control, windowTileMapDisplaySelect)){
		windowMemory = REGION_TWO_BG_MEMORY;
	}
	else {
		windowMemory = REGION_ONE_BG_MEMORY;
	}
	printf("%d\n", windowMemory);
}

static void renderTilesOnBackground(struct gameboy * gameboy)
{
	uint16_t backgroundMemory = 0;
	if (isBitSet(gameboy->screen.control, bgTileDisplaySelect)){
		backgroundMemory = REGION_TWO_BG_MEMORY;
	}
	else {
		backgroundMemory = REGION_ONE_BG_MEMORY;
	}
	printf("%d\n", backgroundMemory);

}

void (*getTileDataGetFunction(struct gameboy * gameboy))(struct gameboy * gameboy)
{
	int index = (int)isBitSet(gameboy->screen.control, bgWindowTileDataSelect);
	return tileDataFuncs[index];
}

static void getTileDataFromRegion1(struct gameboy * gameboy)
{
	//index 0 in func array
	//8800
	//if isBitSet false
}

static void getTileDataFromRegion2(struct gameboy * gameboy)
{
	//index 1
	//8000
	//if isBitSet true
	
}

static void renderSprites(struct gameboy * gameboy)
{

}

static bool windowEnabled(struct gameboy * gameboy)
{
	return isBitSet(gameboy->screen.control, windowDisplayEnable) ? true : false;
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

/*
static bool isInHBlankBounds(struct gameboy * gameboy)
{	
	//mode 0 - probably won't need this function
	return true;
}
*/

static bool isInVBlankBounds(struct gameboy * gameboy)
{
	//mode 1
	uint8_t currentScanline = gameboy->screen.currentScanline;
	return ((currentScanline >= Y) && (currentScanline <= (Y + NO_OF_INVISIBLE_SCANLINES))) ? true : false;
}

static bool isInSpriteAttributeBounds(struct gameboy * gameboy)
{
	//mode 2
	int currentCounter = gameboy->screen.scanlineCounter;
	return ((currentCounter >= SPRITE_ATTR_CYCLE_LOWER) && (currentCounter <= SPRITE_ATTR_CYCLE_UPPER)) ? true : false;
}

static bool isInDriverTransferBounds(struct gameboy * gameboy)
{
	int currentCounter = gameboy->screen.scanlineCounter;
	return ((currentCounter >= DRIVER_TRANSFER_CYCLE_LOWER) && (currentCounter <= DRIVER_TRANSFER_CYCLE_UPPER)) ? true : false;
}

static void handleHBlank(struct gameboy * gameboy)
{
	setBit(&gameboy->screen.status, 1, false);
	setBit(&gameboy->screen.status, 1, true);
	gameboy->screen.currentLCDInterruptEnabled = isBitSet(gameboy->screen.status, HBLANK_ENABLE_BIT);
}
//do some refactoring here
static void handleVBlank(struct gameboy * gameboy)
{
	setBit(&gameboy->screen.status, 1, false);
	setBit(&gameboy->screen.status, 0, true); //set lower 2 bits to 01 (mode 1)
	gameboy->screen.currentLCDInterruptEnabled = isBitSet(gameboy->screen.status, VBLANK_ENABLE_BIT);
}

static void handleSpriteSearch(struct gameboy * gameboy)
{
	setBit(&gameboy->screen.status, 1, true);
	setBit(&gameboy->screen.status, 0, false); //set lower 2 bits to 10
	gameboy->screen.currentLCDInterruptEnabled = isBitSet(gameboy->screen.status, SPRITE_ENABLE_BIT);
}

static void handleTransferToLCDDriver(struct gameboy * gameboy)
{
	setBit(&gameboy->screen.status, 1, true);
	setBit(&gameboy->screen.status, 0, true); //set lower 2 bits to 11
	//this doesn't request an interrupt
}

