#include <stdio.h>
#include <stdlib.h>
#include "../src/lcd.h"
#include "../src/gameboy.h"
#include "../src/cartridge.h"
#include "../src/bitUtils.h"

int main(void)
{
	struct gameboy * gameboy;
	gameboy = createGameboy();
	
	loadGame(gameboy, "../src/tetris.gb");
	gameboy->screen.status = 8;
	updateGraphics(gameboy, 10);
	setBit(&gameboy->screen.control, 7, true);
	setLCDStatus(gameboy);
	
}
