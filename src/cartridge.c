#include "../include/cartridge.h"
#include "../include/gameboy.h"
#include "../include/mbc.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct romInfo romInfoChoices[0x54];
struct ramInfo ramInfoChoices[0x03];
char * locales[0x2];

static bool isValidGame(const char * directory);
static void initialiseRomBankChoices();
static void initialiseRamBankChoices();
static void initialiseLocaleChoices();

void loadGame(struct gameboy * gameboy, const char * directory)
{
	printf("Loading game at \"%s\"... ", directory);
	if (!isValidGame(directory)){
		fprintf(stderr, "Error loading game.\n");
		exit(-1);
	}

	FILE * game;
	game = fopen(directory, "rb");
	if (game == NULL){
		fprintf(stderr, "Game at %s cannot be loaded.\n", directory);
		destroyGameboy(gameboy);
		exit(-1); //implement better error handling system
	}

	fread(gameboy->cartridge.memory, 1, MAX_CART_SIZE, game);
	fclose(game);
	//load first 0x8000 into gameboy memory - do this with MBC functionality
	//memcpy(&gameboy->memory.mem, &gameboy->cartridge.memory, CARTRIDGE_SIZE);

	loadBankType(gameboy);
	loadRomInfo(gameboy);
	loadRamInfo(gameboy);
	loadLocaleInfo(gameboy);

	initialiseRomBanks(gameboy); 

	printf("done.\n");
	
	printCartDetails(gameboy);

}

void loadBankType(struct gameboy * gameboy)
{
	gameboy->cartridge.bankMode = gameboy->cartridge.memory[MBC_MODE_ADDRESS];
	//pokemon red and blue are coming up MBC5 when they are MBC3 in real life

}

void loadRomInfo(struct gameboy * gameboy)
{
	initialiseRomBankChoices();
	uint8_t bankCode = gameboy->cartridge.memory[ROM];
	gameboy->cartridge.romBankCount = romInfoChoices[bankCode].noOfBanks;
	gameboy->cartridge.romSize = romInfoChoices[bankCode].size;
}

static void initialiseRomBankChoices()
{
        romInfoChoices[0x00].size = 32768;
        romInfoChoices[0x00].noOfBanks = 0;
        romInfoChoices[0x01].size = 65536;
        romInfoChoices[0x01].noOfBanks = 4;
        romInfoChoices[0x02].size = 131072;
        romInfoChoices[0x02].noOfBanks = 8;
        romInfoChoices[0x03].size = 262144;
        romInfoChoices[0x03].noOfBanks = 16;
        romInfoChoices[0x04].size = 524288;
        romInfoChoices[0x04].noOfBanks = 32;
        romInfoChoices[0x05].size = 1048576;
        romInfoChoices[0x05].noOfBanks = 64;
        romInfoChoices[0x06].size = 2097152;
        romInfoChoices[0x06].noOfBanks = 128;
        romInfoChoices[0x07].size = 4194304;
        romInfoChoices[0x07].noOfBanks = 256;
        romInfoChoices[0x52].size = 1153433.6;
        romInfoChoices[0x52].noOfBanks = 72;
        romInfoChoices[0x53].size = 1258291.2;
        romInfoChoices[0x53].noOfBanks = 80;
        romInfoChoices[0x54].size = 1572864;
        romInfoChoices[0x54].noOfBanks = 96;

}

static void initialiseRamBankChoices()
{
	ramInfoChoices[0x00].size = 0;
	ramInfoChoices[0x00].noOfBanks = 0;
	ramInfoChoices[0x01].size = 2048;
	ramInfoChoices[0x01].noOfBanks = 1;
	ramInfoChoices[0x02].size = 8192;
	ramInfoChoices[0x02].noOfBanks = 1;
	ramInfoChoices[0x03].size = 32768;
	ramInfoChoices[0x03].noOfBanks = 4;
	ramInfoChoices[0x04].size = 131072;
	ramInfoChoices[0x04].noOfBanks = 16;
	
}

void loadRamInfo(struct gameboy * gameboy)
{
	initialiseRamBankChoices();
	uint8_t bankCode = gameboy->cartridge.memory[EXTERNAL_RAM];
        gameboy->cartridge.ramBankCount = ramInfoChoices[bankCode].noOfBanks;
        gameboy->cartridge.ramSize = ramInfoChoices[bankCode].size;
	
}

void loadLocaleInfo(struct gameboy * gameboy)
{
	initialiseLocaleChoices();
	int localeCode= gameboy->cartridge.memory[LOCALE];
	gameboy->cartridge.locale = locales[localeCode];
}

static void initialiseLocaleChoices()
{
	locales[0] = "Japanese";
	locales[1] = "Non-Japanese";
}

void printCartDetails(struct gameboy * gameboy)
{
	printf("\nCART DETAILS:\n\n");
	printf("\tCart name: ");
	for (int i = 0x134; i <= 0x143; i += 0x01){
		printf("%c", gameboy->cartridge.memory[i]);
	}
	printf("\n");
	
	//printf("\tBank code: %x\n", gameboy->memory.bank);
	printf("\tROM size: %d bytes\n", gameboy->cartridge.romSize);
	printf("\tRAM size: %d bytes\n", gameboy->cartridge.ramSize);
	printf("\tLocale: %s\n", gameboy->cartridge.locale);

}

//do this later
static bool isValidGame(const char * directory)
{
	return true;
}
