#include "../include/lcd.h"
#include "../include/display.h"
#include "../include/bitUtils.h"
#include "../include/gameboy.h"
#include "../include/memory.h"
#include <stdio.h>
#include <unistd.h>

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

static struct colour getColour(struct gameboy * gameboy, uint8_t colourNum, uint16_t colourAddress);

const void (*scanModeFuncs[4]) = {
	handleHBlank,
	handleVBlank,
	handleSpriteSearch,
	handleTransferToLCDDriver
};

struct colour white = {255, 255, 255};
struct colour lightGrey = {0xCC, 0xCC, 0xCC};
struct colour darkGrey = {0x77, 0x77, 0x77};
struct colour black = {0, 0, 0};

//move this
struct colour palette[4] = {
	{255, 255, 255}, 
	{0xCC, 0xCC, 0xCC}, 
	{0x77, 0x77, 0x77}, 
	{0, 0, 0}
};

struct colour frameBuffer[X * Y];

static void bufferPixel(struct gameboy * gameboy, int row, int col, struct colour colour)
{
	gameboy->screen.frameBuffer[X * row + col] = colour;
}

void updateGraphicsTest(struct gameboy * gameboy)
{
	//fill a frame buffer with some simple pixel data and see if it draws correctly
	static int count;
	bufferPixel(gameboy, 5, 10, lightGrey);
	bufferPixel(gameboy, 50, 10, black);
	//printf("status before: ");
	//printBinFromDec(gameboy->screen.status);
	setLCDStatus(gameboy);
	if (isLCDEnabled(gameboy)){
		gameboy->screen.scanlineCounter += gameboy->cpu.previousInstruction.cycles;
	}
	//printf("status after: ");
	//printBinFromDec(gameboy->screen.status);
	++count;
	printf("%d\n", gameboy->screen.scanlineCounter);
	if (count > 2){
		//exit(-1);
	}
	
}

void updateGraphics(struct gameboy * gameboy)
{
	int cycles = gameboy->cpu.previousInstruction.cycles;
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
		//printf("LCD isn't enabled - starting new scanline\n");
		startNewScanline(gameboy);
	}
	else {
	//	printf("LCD is enabled\n");
		if (isInVBlankBounds(gameboy)){
			//printf("Handle vblank\n");
			handleVBlank(gameboy);
		}
		else if (isInSpriteAttributeBounds(gameboy)){
			//printf("Handle OAM\n");
			handleSpriteSearch(gameboy);
		}
		else if (isInDriverTransferBounds(gameboy)){
			//printf("handle transfer\n");
			handleTransferToLCDDriver(gameboy);
		}
		else {
			//printf("handle hblank\n");
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
	//forgive me father for I have sinned
	//this and renderSprites copied from codeslinger tut, needs major refactor
	//I'm just desperate to get something on screen at this point
	uint16_t tileData = 0;
	uint16_t backgroundMemory = 0;
	bool unsig = true;
	bool usingWindow = false;
	
	if (windowEnabled(gameboy)){
		if (gameboy->screen.windowYPos <= gameboy->screen.currentScanline){
			usingWindow = true;
		}
	}

	//which tile data are we using?
	if (isBitSet(gameboy->screen.control, 4)){
		tileData = 0x8000;
	}
	else {
		//signed
		tileData = 0x8800;
		unsig = false;
	}

	//which background memory?
	if (!usingWindow){
		if (isBitSet(gameboy->screen.control, 3)){
			backgroundMemory = 0x9C00;
		}
		else {
			backgroundMemory = 0x9800;
		}
	}
	else {
		//which window memory?
		if (isBitSet(gameboy->screen.control, 6)){
			backgroundMemory = 0x9C00;
		}
		else {
			backgroundMemory = 0x9800;
		}
	}

	uint8_t yPos = 0; //used to calculate which of the 32 vertical tiles is being drawn
	if (!usingWindow){
		yPos = gameboy->screen.scrollY + gameboy->screen.currentScanline;
	}
	else {
		yPos = gameboy->screen.currentScanline - gameboy->screen.scrollY;
	}

	//which of the 8 vertical pixels of the current tile is the scanline on?
	uint16_t tileRow = (yPos / 8) * 32;
	
	//draw 160 horizontal pixels for this scanline
	for (int pixel = 0; pixel < X; pixel++){
		uint8_t xPos = pixel + gameboy->screen.scrollX;
		if (usingWindow && (pixel >= gameboy->screen.windowXPos - 7)){
			xPos = pixel - (gameboy->screen.windowXPos - 7);
		}
		
		//which of the 32 horiz. tiles does this xPos fall within?
		uint16_t tileCol = xPos / 8;
		int16_t tileNum;

		//get tileID
		uint16_t tileAddress = backgroundMemory + tileRow + tileCol;
		if (unsig){
			tileNum = readByte(gameboy, tileAddress);
		}
		else {
			tileNum = (int16_t)readByte(gameboy, tileAddress);
		}

		//find where this tileID is in memory.
		uint16_t tileLocation = tileData;
		if (unsig){
			tileLocation += (tileNum * 16);
		}
		else {
			tileLocation += ((tileNum + 128) * 16);
		}

		//find current vertical line on the tile we're on to get tile data
		//from memory
		uint8_t line = yPos % 8;
		line *= 2; //each vertical line takes up 2 bytes
		uint8_t data1 = readByte(gameboy, tileLocation + line);
		uint8_t data2 = readByte(gameboy, tileLocation + line + 1);
	
		//pixel 0 in the tile is bit 7 of data1 and 2
		//pixel 1 is 6 etc.

		int colourBit = xPos % 8;
		colourBit -= 7;
		colourBit *= -1;

		//combine data 2 and 1 to get the colour ID for this pixel
		int colourNum = getBit(data2, colourBit);
		colourNum <<= 1;
		colourNum |= getBit(data1, colourBit);

		//now we have the colour ID, get the actual colour from 0xFF47
		struct colour colour = getColour(gameboy, colourNum, 0xFF47);

		gameboy->screen.frameBuffer[(pixel * Y) + gameboy->screen.currentScanline] = colour;
	}
	
}

static struct colour getColour(struct gameboy * gameboy, uint8_t colourNum, uint16_t colourAddress)
{
	uint8_t pal = readByte(gameboy, colourAddress);
	int hi = 0;
	int lo = 0;

	switch(colourNum){
		case 0: hi = 1; lo = 0;
		break;
		case 1: hi = 3; lo = 2;
		break;
		case 2: hi = 5; lo = 4;
		break;
		case 3: hi = 7; lo = 6;
	}

	int colour = 0;
	colour = getBit(pal, hi) << 1;
	colour |= getBit(pal, lo);

	return palette[colour];
}

static void renderSprites(struct gameboy * gameboy)
{
	bool use8x16 = isBitSet(gameboy->screen.control, 2);
	for (int sprite = 0; sprite < 40; sprite++){
		//there are 40 sprites, loop through all of them
		uint8_t index = sprite * 4; //each sprite occupies 4 bytes in attr table
		//just type now, ask questions later
		uint8_t yPos = readByte(gameboy, 0xFE00 + index) - 16;
		uint8_t xPos = readByte(gameboy, 0xFE00 + index + 1) - 8;
		uint8_t tileLocation = readByte(gameboy, 0xFE00 + index + 2);
		uint8_t attributes = readByte(gameboy, 0xFE00 + index + 3);

		bool yFlip = isBitSet(attributes, 6);
		bool xFlip = isBitSet(attributes, 5);

		int scanline = gameboy->screen.currentScanline;
	
		int ySize = 8;
		if (use8x16){
			ySize = 16;
		}

		//does this sprite intercept with the scanline?
		if ((scanline >= yPos) && (scanline < (yPos + ySize))){
			int line = scanline - yPos;

			//read the sprite in backwards in the y axis
			if (yFlip){
				line -= ySize;
				line *= -1;
			}

			line *= 2; //same as for tiles
			uint16_t dataAddress = (0x8000 + (tileLocation * 16)) + line;
			uint8_t data1 = readByte(gameboy, dataAddress);
			uint8_t data2 = readByte(gameboy, dataAddress + 1);
	
			//easier to read in from right to left as pixel 0 is bit 7 
			//in colour data, pixel 1 is bit 6 etc...

			for (int tilePixel = 7; tilePixel >= 0; tilePixel--){
				int colourBit = tilePixel;
				if (xFlip){
					colourBit -= 7;
					colourBit *= -1;
				}
				//the rest is the same for tiles
				int colourNum = getBit(data2, colourBit);
				colourNum <<= 1;
				colourNum |= getBit(data1, colourBit);
				
				uint16_t colourAddress = isBitSet(attributes, 4) ? 0xFF49 : 0xFF48;
				struct colour colour = getColour(gameboy, colourNum, colourAddress);

				int xPix = 0 - tilePixel;
				xPix += 7;
				int pixel = xPos + xPix;
				
				int pixelOffset = (scanline * Y) + pixel; 

				gameboy->screen.frameBuffer[pixelOffset] = colour;
			}
		}
	}
				
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
	//gameboy->screen.status &= 252; //whats going on?
	setBit(&gameboy->screen.control, lcdEnable, true);
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

