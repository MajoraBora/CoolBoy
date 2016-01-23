#include "../include/lcd.h"
#include "../include/display.h"
#include "../include/bitUtils.h"
#include "../include/gameboy.h"
#include "../include/memory.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>

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

static enum COLOUR getColourEnum(struct gameboy * gameboy, uint8_t colourNum, uint16_t address);

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

void updateGraphicsTest(struct gameboy * gameboy)
{
	//fill a frame buffer with some simple pixel data and see if it draws correctly
	setLCDStatus(gameboy);
	uint8_t previousMode = getCurrentMode(gameboy);
	requestAnyNewModeInterrupts(gameboy, previousMode);
	if (isLCDEnabled(gameboy)){
		gameboy->screen.scanlineCounter += gameboy->cpu.previousInstruction.cycles;
	}
	else {
		return;
	}
	
	if (gameboy->screen.scanlineCounter >= SCANLINE_CYCLE_TIME){
		gameboy->screen.currentScanline++;
		gameboy->screen.scanlineCounter = 0;
		if (gameboy->screen.currentScanline == Y){
			requestInterrupt(gameboy, int_vblank);
			//printf("start vblank\n");
		}
		else if (gameboy->screen.currentScanline > (Y + NO_OF_INVISIBLE_SCANLINES)){
			gameboy->screen.currentScanline = 0;
			//printf("resetting scanline\n");
		}
		else if (gameboy->screen.currentScanline < Y){
//			printf("drawing scanline %d\n", gameboy->screen.currentScanline);
			drawScanline(gameboy);
		}
		else {
			//printf("In vblank\n");
		}
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
		requestInterrupt(gameboy, int_vblank);
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
	//printf("draw scanline\n");
	if (backgroundTilesEnabled(gameboy)){
	//	printf("Rendering tiles\n");
		renderTiles(gameboy);
	}
	
	if (spritesEnabled(gameboy)){
	//	printf("Rendering Sprites\n");
		//renderSprites(gameboy);
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

	//where to draw the visual area and the window
	uint8_t scrollY = readByte(gameboy, 0xFF42); //the Y origin of the visible 160x144 pixel area in the BG 256x256 map
	uint8_t scrollX = readByte(gameboy, 0xFF43); //the X coord of the scroll
	uint8_t windowY= readByte(gameboy, 0xFF4A);
	uint8_t windowX = readByte(gameboy, 0xFF4B) - 7;

	bool usingWindow = false;

	if (isBitSet(gameboy->screen.control, 5)){ //is the window enabled?
		if (windowY <= gameboy->screen.currentScanline){ //is the scanline within the window?
			//is the current scanline 
			usingWindow = true;
		}
	}

	if (isBitSet(gameboy->screen.control, 4)){ //which tile data?
		tileData = 0x8000;
	}
	else {
		tileData = 0x8800;
		unsig = false; //tileData at 0x8800 is signed
	}

	if (!usingWindow){
		if (isBitSet(gameboy->screen.control, 3)){ //which background memory for tiles?
			backgroundMemory = 0x9C00;
		}
		else {
			backgroundMemory = 0x9800;
		}
	}
	else {
		if (isBitSet(gameboy->screen.control, 6)){ //which window memory for tiles?
			backgroundMemory = 0x9C00;
		}
		else {
			backgroundMemory = 0x9800;
		}
	}

	uint8_t yPos = 0; //which vertical line the current scanline is drawing
	
	if (!usingWindow){
		yPos = scrollY + gameboy->screen.currentScanline;
	}
	else {
		yPos = gameboy->screen.currentScanline - windowY;
	}

	//which of the 8 vertical pixels of the current tile is the scanline on?
	uint16_t vertTilePixel = ((uint8_t)(yPos/8)) * 32;

	//draw 160 horizontal pixels for the current scanline
	for (int pixel = 0; pixel < 160; pixel++){
		uint8_t xPos = pixel + scrollX;
		//translate current xPos to window if necessary
		if (usingWindow){
			if (pixel >= windowX){
				xPos = pixel - windowX;
			}
		}

		//which of the 32 horizontal tiles does this xPos fall within?

		uint16_t horTileColumn = xPos/8;
		int16_t tileNum; //the tile identity number
		uint16_t tileAddress = backgroundMemory + vertTilePixel + horTileColumn;

		if (unsig){
			tileNum = (uint8_t)readByte(gameboy, tileAddress);
		}
		else {
			tileNum = (int8_t)readByte(gameboy, tileAddress);
		}

		//find the tile in memory
		uint16_t tileLocation = tileData;

		if (unsig){
			tileLocation += (tileNum * 16);
		}
		else {
			tileLocation += ((tileNum + 128) * 6);
		}

		//find the vertical line on the tile the scanline is at
		uint8_t line = yPos % 8;
		line *= 2; //each vertical line takes up 2 bytes
		uint8_t data1 = readByte(gameboy, tileLocation + line);
		uint8_t data2 = readByte(gameboy, tileLocation + line + 1);
	
		//pixel 0 in the tile is bit 7 or data 1 and data 2, pixel 1 is bit 6 etc...
		int colourBit = xPos % 8;
		colourBit -= 7;
		colourBit *= -1;

		int colourNum = getBit(data2, colourBit);
		colourNum <<= 1;
		colourNum |= getBit(data1, colourBit);

		//get the actual colour from the palette using the colour ID

		enum COLOUR colour = getColourEnum(gameboy, colourNum, 0xFF47);
		int red = 0;
		int green = 0;
		int blue = 0;
		
		switch(colour){
			case WHITE: red = 255; green = 255; blue = 255; break;
			case LIGHT_GREY: red = 0xCC; green = 0xCC; blue = 0xCC; break;
			case DARK_GREY: red = 0x77; green = 0x77; blue = 0x77; break;
			case BLACK: break;
		}

		/*gameboy->screen.frameBuffer3D[pixel][gameboy->screen.currentScanline][0] = red;
		gameboy->screen.frameBuffer3D[pixel][gameboy->screen.currentScanline][1] = green;
		gameboy->screen.frameBuffer3D[pixel][gameboy->screen.currentScanline][2] = blue;
		*/
		/*
		gameboy->screen.frameBuffer3D[49][99][0] = 255;
		gameboy->screen.frameBuffer3D[49][99][1] = 255;
		gameboy->screen.frameBuffer3D[49][99][2] = 255;
		gameboy->screen.frameBuffer3D[49][100][0] = 255;
		gameboy->screen.frameBuffer3D[49][100][1] = 255;
		gameboy->screen.frameBuffer3D[49][100][2] = 255;
		*/
		/*
		int x1 = 50;
		int y1 = 100;
		int x2 = 51;
		int y2 = 100;
		int width = 160;
		gameboy->screen.frameBufferNew[3 * (y1 * width + x1) + 0] = 255;
		gameboy->screen.frameBufferNew[3 * (y1 * width + x1) + 1] = 255;
		gameboy->screen.frameBufferNew[3 * (y1 * width + x1) + 2] = 255;
		gameboy->screen.frameBufferNew[3 * (y2 * width + x2) + 0] = 255;
		gameboy->screen.frameBufferNew[3 * (y2 * width + x2) + 1] = 255;
		gameboy->screen.frameBufferNew[3 * (y2 * width + x2) + 2] = 255;
		*/

		gameboy->screen.frameBufferNew[3 * (gameboy->screen.currentScanline * X + pixel) + 0] = red;
		gameboy->screen.frameBufferNew[3 * (gameboy->screen.currentScanline * X + pixel) + 1] = green;
		gameboy->screen.frameBufferNew[3 * (gameboy->screen.currentScanline * X + pixel) + 2] = blue;
	}
}


static enum COLOUR getColourEnum(struct gameboy * gameboy, uint8_t colourNum, uint16_t address)
{
	enum COLOUR result = WHITE;
	uint8_t palette = readByte(gameboy, address);
	int hi = 0;
	int lo = 0;

	//which bits of the colour palette does the colour id map to?
	switch(colourNum)
	{
		case 0: hi = 1; lo = 0; break;
		case 1: hi = 3; lo = 2; break;
		case 2: hi = 5; lo = 4; break;
		case 3: hi = 7; lo = 6; break;
	}

	//use the palette to get the colour
	int colour = 0;
	colour = getBit(palette, hi) << 1;
	colour |= getBit(palette, lo);

	//convert the game colour to emulator colour
	switch(colour)
	{
		case 0: result = WHITE; break;
		case 1: result = LIGHT_GREY; break;
		case 2: result = DARK_GREY; break;
		case 3: result = BLACK; break;
	}

	return result;
}	

static void renderSprites(struct gameboy * gameboy)
{
	//40 tiles in 0x8000 - 0x8FFF
	//scan through them all, check their attributes to find where they need to be rendered
	//sprite attributes found in the sprite attribute table 0xFE00-0xFE9F
	//each sprite has 4 bytes of attributes: y pos, x pos, sprite id, attributes

	bool use8x16 = false;
	if (isBitSet(gameboy->screen.control, 2)){
		use8x16 = true; //using sprite size of 8x16 pixels
	}
	
	for (int sprite = 0; sprite < 40; sprite++){
		//for each of the 40 sprites
		uint8_t spriteIndex = sprite * 4; //sprite occupies 4 bytes in the sprite attribute table
		uint8_t yPos = readByte(gameboy, (uint16_t)(0xFE00 + spriteIndex)) - 16; //index into sprite table, zero Y coord (offset by height)
		uint8_t xPos = readByte(gameboy, (uint16_t)(0xFE00 + spriteIndex + 1)) - 8; //index into sprite table, zero X coord (offset by width)
		uint8_t tileNum = readByte(gameboy, (uint16_t)(0xFE00 + index + 2));
		uint8_t attributes = readByte(gameboy, (uint16_t)(0xFE00 + index + 3));

		/* attributes table:
			bit 7: sprite to background priority
			bit 6: Y flip
			bit 5: x flip
			bit 4: colour palette number
			bit 3: not used in standard gameboy
			bit 2-0: not used in standard gameboy	
		*/

		//is the sprite flipped?
		bool yFlip = isBitSet(attributes, 6);
		bool xFlip = isBitSet(attributes, 5);

		uint8_t scanline = gameboy->screen.currentScanline;

		//store height of sprite
		int ySize = 8;
		if (use8x16){
			ySize = 16;
		}

		//is the scanline intercepting the sprite?
		if ((scanline >= yPos) && (scanline < (yPos + ySize))){
			//the vertical line on the sprite the scanline is at
			int line = scanline - yPos;

			if (yFlip){
				line -= ySize;
				line *= -1;
			}

			line *= 2; //two bytes, same as tiles
			uint16_t dataAddress = (0x8000 + (tileNum * 16)) + line;
			uint8_t data1 = readByte(gameboy, dataAddress);
			uint8_t data2 = readByte(gameboy, dataAddress+1);

			//read from right to left, as pixel 0 is bit 7 etc.
			for (int pixel = 7; pixel >= 0; pixel--){
				int colourBit = pixel;
				if (xFlip){
					colourBit -= 7;
					colourBit *= -1;
				}
	
				//the rest is the same as for tiles
				int colourNum = getBit(data2, colourBit);
				colourNum <<= 1;
				colourNum |= getBit(data1, colourBit);
				
				uint16_t colourAddress = isBitSet(attributes, 4) ? 0xFF49 : 0xFF48;
				enum COLOUR colour = getColourEnum(gameboy, colourNum, colourAddress);

				//white is transparent for sprites
				if (colour == WHITE){
					continue;
				}

				int red = 0;
				int green = 0;
				int blue = 0;

				switch(colour)
				{
					case WHITE: break;
					case LIGHT_GREY: red = 0xCC; green = 0xCC; blue = 0xCC; break;
					case DARK_GREY: red = 0x77; green = 0x77; blue = 0x77; break;
					case BLACK: break;
				}

				int xPix = 0 - pixel;
				xPix += 7;

				int pixel = xPos + xPix;
	
				gameboy->screen.frameBufferNew[3 * (gameboy->screen.currentScanline * X + pixel) + 0] = red;
				gameboy->screen.frameBufferNew[3 * (gameboy->screen.currentScanline * X + pixel) + 1] = green;
				gameboy->screen.frameBufferNew[3 * (gameboy->screen.currentScanline * X + pixel) + 2] = blue;
	
					
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

