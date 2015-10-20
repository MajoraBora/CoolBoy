#include "cartridge.h"
#include "gameboy.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static bool isValidGame(const char * directory);

void loadGame(struct gameboy * gameboy, const char * directory)
{
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

	for (int i = 0; i < MAX_CART_SIZE; i++){
		printf("%d ", gameboy->memory.cart[i]);
	}
}

//do this later
static bool isValidGame(const char * directory)
{
	return true;
}
