#include "../include/joypad.h"
#include "../include/gameboy.h"
#include "../include/bitUtils.h"
#include "../include/interrupt.h"
#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>

struct buttonMap buttons[NO_OF_BUTTONS] = {
	{UP, SDLK_w}, 
	{DOWN, SDLK_s},
	{LEFT, SDLK_a},
	{RIGHT, SDLK_d},
	{A, SDLK_i},
	{B, SDLK_j},
	{START, SDLK_RETURN},
	{SELECT, SDLK_SPACE}
	
};

//use to map individual buttons to correct bit in reg
static uint8_t sharedButtonBitValues[NO_OF_BUTTONS] = {
	2, 3, 1, 0, 0, 1, 3, 2
}; //dirty

static enum regBit getCorrectSelectBit(enum button buttonIndex);
static bool interruptableStateChanged(uint8_t prev, uint8_t cur);
static bool keyReleased(uint8_t prev, uint8_t cur);
static void doInterruptIfAllowed(struct gameboy * gameboy, enum button buttonIndex);
static void resetIfAllowed(struct gameboy * gameboy, enum button buttonIndex);
static bool isDirectionalButton(enum button buttonIndex);
static bool isStandardButton(enum button buttonIndex);

void handleKeys(SDL_Event * event, int * quit)
{
	if (event->type == SDL_KEYDOWN){
		if (event->key.keysym.sym == SDLK_ESCAPE){
			*quit = 1;
		}
	}
}

void startKeyModule(struct gameboy * gameboy)
{
	SDL_Event event;
	int quit = 0;
	Uint8 * keys = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("error.\n");
		exit(-1);
	}

	if (!SDL_SetVideoMode(255, 255, 0, 0)){
		printf("error.\n");
		SDL_Quit();
		exit(-1);
	}

	SDL_EnableUNICODE(1);

	while(!quit){
		while(SDL_PollEvent(&event)){
			keys = SDL_GetKeyState(NULL);

			if (keys[SDLK_ESCAPE]){
				quit = 1;
				continue;
			}

			if (event.type == SDL_KEYUP || event.type == SDL_KEYDOWN){
				updateJoypadState(gameboy, keys);
			}

		}
	}
}

void updateJoypadState(struct gameboy * gameboy, Uint8 * keys)
{
	for (int i = 0; i < NO_OF_BUTTONS; i++){
		struct buttonMap currentButton = buttons[i]; //{button, mapped sdl button}
		enum button buttonIndex = currentButton.button;
		SDLKey sdlKeyIndex = currentButton.sdlKey;

		uint8_t previousState = getBit(gameboy->joypad.buttonState, buttonIndex);
		uint8_t currentState = !keys[sdlKeyIndex]; //keys sets value to 1 when pressed, reverse this for gameboy

		setBit(&gameboy->joypad.buttonState, buttonIndex, currentState); 

		if (interruptableStateChanged(previousState, currentState)){
			doInterruptIfAllowed(gameboy, buttonIndex);
		}
		else if (keyReleased(previousState, currentState)){
			printf("key released\n");
			resetIfAllowed(gameboy, buttonIndex);
		}
	}

}

static bool interruptableStateChanged(uint8_t prev, uint8_t cur)
{
	return (prev == 1) && (cur == 0);
}

static void doInterruptIfAllowed(struct gameboy * gameboy, enum button buttonIndex)
{
	uint8_t reg = gameboy->memory.mem[0xFF00];
	
	//!isBitSet because a 0 means it is selected
	if (isDirectionalButton(buttonIndex)){
		if (!isBitSet(reg, DIRECTION_SELECT)){
			//printf("Directional interrupt\n");
			setBit(&reg, sharedButtonBitValues[buttonIndex], false);
			writeByte(gameboy, JOYPAD_REG, reg);
			requestInterrupt(gameboy, joypad);
		}
	}
	else if (isStandardButton(buttonIndex)){
		if (!isBitSet(reg, BUTTON_SELECT)){
			setBit(&reg, sharedButtonBitValues[buttonIndex], false);
			writeByte(gameboy, JOYPAD_REG, reg);
			requestInterrupt(gameboy, joypad);
			printf("Standard button interrupt\n");
		}
	}
	else {
		fprintf(stderr, "Error regarding Joypad interrupts.\n");
	}
	printf("joypad reg: ");
	printBinFromDec(reg);
}

static bool keyReleased(uint8_t prev, uint8_t cur)
{
	return (prev == 0) && (cur == 1);
}

static void resetIfAllowed(struct gameboy * gameboy, enum button buttonIndex)
{
	//check correct bit for button. If not set (ie if enabled), go ahead and reset. If it is set, don't do it
	uint8_t reg = gameboy->memory.mem[0xFF00];
	printf("reg at resetIfAllowed: ");
	printBinFromDec(reg);
	uint8_t regBit = sharedButtonBitValues[buttonIndex];
	printf("reg bit to use: %d\n", regBit);
	enum regBit selectBit = getCorrectSelectBit(buttonIndex);
	printf("select bit: %d\n", selectBit);
	if (!isBitSet(reg, selectBit)){
		//if 0 (selected), reset
		setBit(&reg, regBit, true);
		printf("reg after reset: ");
		printBinFromDec(reg);
		gameboy->memory.mem[0xFF00] = reg;
	}
		
}

static enum regBit getCorrectSelectBit(enum button buttonIndex)
{
	if (isDirectionalButton(buttonIndex)){
		return DIRECTION_SELECT;
	}
	else {
		return BUTTON_SELECT;
	}
}

static bool isDirectionalButton(enum button buttonIndex)
{
	return (buttonIndex >= RIGHT) && (buttonIndex <= UP);
}

static bool isStandardButton(enum button buttonIndex)
{
	return (buttonIndex >= SELECT) && (buttonIndex <= A);
}

