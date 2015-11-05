#include "lcd.h"
#include "bitUtils.h"
#include "gameboy.h"

void updateGraphics(struct gameboy * gameboy, int cycles)
{
	if (isLCDEnabled(gameboy)){
		//handleVBlank(gameboy);
		//work out what to do here
	}
}

bool isLCDEnabled(struct gameboy * gameboy)
{
	//refactor -replace this with a bool field in struct, lcdEnabled or something
	return isBitSet(gameboy->screen.control, lcdEnable);
}
