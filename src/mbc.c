#include "../include/mbc.h"
#include "../include/gameboy.h"
#include <string.h>
#include <stdio.h>

static void initialiseBankZero(struct gameboy * gameboy, int size);
static void initialiseRomOnly(struct gameboy * gameboy);
static void initialiseMBC1(struct gameboy * gameboy);
static void initialiseMBC2(struct gameboy * gameboy);
//static void initialiseMBC3(struct gameboy * gameboy);
//static void initialiseMBC4(struct gameboy * gameboy);
//static void initialiseMBC5(struct gameboy * gameboy);

void initialiseRomBanks(struct gameboy * gameboy)
{
	//switch statement to see what initialisation function to run
	//change this to an array of function pointers as support for different 
	//bank methods increases

	switch(gameboy->cartridge.bankMode){
		case ROM_ONLY:
			initialiseRomOnly(gameboy);
			break;
		case MBC1:
			initialiseMBC1(gameboy);
			break;
		case MBC1_RAM:
			initialiseMBC1(gameboy);
			break;
		case MBC2:
			initialiseMBC2(gameboy);
			break;
		case MBC2_BATTERY:
			initialiseMBC2(gameboy);
			break;
		default:
			printf("Unsupported MBC type %d\n", gameboy->cartridge.bankMode);
			break;
	}
}

static void initialiseRomOnly(struct gameboy * gameboy)
{
	initialiseBankZero(gameboy, ROM_ONLY_CARTRIDGE_SIZE);
}

static void initialiseMBC1(struct gameboy * gameboy)
{
	initialiseBankZero(gameboy, BANK_ZERO_SIZE);
}

static void initialiseMBC2(struct gameboy * gameboy)
{
	initialiseBankZero(gameboy, BANK_ZERO_SIZE);
}

/*
static void initialiseMBC3(struct gameboy * gameboy)
{
	initialiseBankZero(gameboy, BANK_ZERO_SIZE);
}

static void initialiseMBC4(struct gameboy * gameboy)
{
	initialiseBankZero(gameboy, BANK_ZERO_SIZE);
}

static void initialiseMBC5(struct gameboy * gameboy)
{
	initialiseBankZero(gameboy, BANK_ZERO_SIZE);
}
*/

static void initialiseBankZero(struct gameboy * gameboy, int size)
{
	//copy first 16kB of cartridge to 0000-3FFF
	memcpy(&gameboy->memory.mem, &gameboy->cartridge.memory, size);
}
