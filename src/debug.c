#include "../include/debug.h"
#include "../include/gameboy.h"
#include "../include/timer.h"
#include "../include/memory.h"
#include <stdio.h>
#include <stdlib.h>

void printDebugTrace(struct gameboy * gameboy)
{
	printf("\n");
	printf("**DEBUG TRACE**\n\n");
	printf("--CPU--\n");
	printf("\tStack Pointer: %x\n", gameboy->cpu.sp);
	printf("\tProgram Counter: %x\n", gameboy->cpu.pc);
	printf("\tAF: %x\n", gameboy->cpu.af);
	printf("\tBC: %x\n", gameboy->cpu.bc);
	printf("\tDE: %x\n", gameboy->cpu.de);
	printf("\tHL: %x\n", gameboy->cpu.hl);

	printf("\n");
	printf("--SCREEN--\n");
	printf("\tLCD Control Register: %x\n", gameboy->screen.control);
	printf("\tLCD Status Register: %x\n", gameboy->screen.status);
	printf("\tScroll X: %x\n", gameboy->screen.scrollX);
	printf("\tScroll Y: %x\n", gameboy->screen.scrollY);
	printf("\tCurrent Scanline: %x\n", gameboy->screen.currentScanline);
	printf("\tScanline Counter: %x\n", gameboy->screen.scanlineCounter);
	printf("\tWindow X: %x\n", gameboy->screen.windowXPos);
	printf("\tWindow Y: %x\n", gameboy->screen.windowYPos);

	printf("\n");
	printf("--INTERRUPTS--\n");
	printf("\tMaster Enabled: %s\n", (gameboy->interrupts.masterEnable ? "yes" : "no"));
	printf("\tInterrupt Enable Register: %x\n", gameboy->interrupts.intEnable);
	printf("\tInterrupt Request Register: %x\n", gameboy->interrupts.intRequest);
	
	printf("\n");
	printf("--TIMERS--\n");
	printf("\tTMA: %x\n", gameboy->memory.mem[TMA]);
	printf("\tTIMA: %x\n", gameboy->memory.mem[TIMA]);
	printf("\tDivider Register: %x\n", gameboy->memory.mem[DIV_REG]);
}
