#include "cartridge.h"
#include "gameboy.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static bool isValidGame(const char * directory);

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

	fread(gameboy->memory.cart, 1, MAX_CART_SIZE, game);
	fclose(game);

	loadBankType(gameboy);
	printf("done.\n");
	
	printCartDetails(gameboy);

}

void loadBankType(struct gameboy * gameboy)
{
	gameboy->memory.bank = gameboy->memory.cart[MBC_MODE_ADDRESS];

}

void printCartDetails(struct gameboy * gameboy)
{
	printf("\nCART DETAILS:\n\n");
	printf("\tCart name: ");
	for (int i = 0x134; i <= 0x143; i += 0x01){
		printf("%c ", gameboy->memory.cart[i]);
	}
	printf("\n");
	
	printf("\tBank code: %x\n", gameboy->memory.bank);
	printf("\tROM size code: %x\n", gameboy->memory.cart[0x148]);
	printf("\tRAM size code: %x\n", gameboy->memory.cart[0x149]);
	printf("\tDestination code: %x\n", gameboy->memory.cart[0x14A]);

}

//do this later
static bool isValidGame(const char * directory)
{
	return true;
}
