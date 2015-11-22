#include "../include/joypad.h"
#include "../include/gameboy.h"
#include "../include/bitUtils.h"
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

static bool interruptableStateChanged(uint8_t prev, uint8_t cur);

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
	//if a key is pressed, its value in the register is 0, not 1

	/*
	Whenever the game reads from 0xFF00 i trap it with ReadMemory and call a 
	function which looks at memory address 0xFF00 to see if the game is interest 
	in directional buttons or standard buttons and then I return a byte data 
	which combines m_JoypadState with 0xFF00 so the game gets the correct state 
	of the joypad. (codeslinger)
	*/

	/*
	Maintain an 8 bit int, emulating the joypad register at 0xFF00
	
	maintain 8 bit int, where each bit represents the state of a button - initialise all to 1
	
	
	*/

	for (int i = 0; i < NO_OF_BUTTONS; i++){
		struct buttonMap currentButton = buttons[i]; //{button, mapped sdl button}
		enum button buttonIndex = currentButton.button;
		SDLKey sdlKeyIndex = currentButton.sdlKey;

		uint8_t previousState = getBit(gameboy->joypad.buttonState, buttonIndex);
		uint8_t currentState = !keys[sdlKeyIndex]; //keys sets value to 1 when pressed, reverse this for gameboy

		setBit(&gameboy->joypad.buttonState, buttonIndex, currentState); 
		printf("%d ", currentState);

		if (interruptableStateChanged(previousState, currentState)){
			printf("interrupt %d if possible\n", i);
		}
	
		if (gameboy->joypad.state[currentButton.button] == 1 && gameboy->joypad.previousState[currentButton.button] == 0){
			if (currentButton.button < RIGHT && gameboy->joypad.buttonMode){
				//if current active button is non-directional and button mode is active
				//fire interrupt
				printf("button interrupt\n");
			}
			else if (currentButton.button >= RIGHT && !gameboy->joypad.buttonMode){
				//if current active button is directional and directional mode is active
				//fire interrupt
				printf("directional interrupt\n");
			}
		}
	}

	printf("\n");


}

static bool interruptableStateChanged(uint8_t prev, uint8_t cur)
{
	return (prev == 1) && (cur == 0);
}
