#include "../include/joypad.h"
#include "../include/gameboy.h"
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
	//SDL_EnableKeyRepeat(0, 40);

	while(!quit){
		while(SDL_PollEvent(&event)){
			keys = SDL_GetKeyState(NULL);

			if (keys[SDLK_ESCAPE]){
				quit = 1;
				continue;
			}

			if (event.type == SDL_KEYUP){
				printf("%d\n", buttons[0].sdlKey);
				printf("%d\n", SDLK_w);
				updateJoypadState(gameboy, keys);
			}

			/*
			switch(event.type){
				case SDL_KEYDOWN:
				break;
				case SDL_KEYUP:
				break;
			}
			if (keys[SDLK_ESCAPE]){
				quit = 1;
			}
	
			if (keys[SDLK_i]){
				printf("A Button pressed.\n");
			}

			if (keys[SDLK_j]){
				printf("B Button pressed.\n");
			}

			if (keys[SDLK_w]){
				printf("UP pressed\n");
			}
			else if (keys[SDLK_s]){
				printf("DOWN pressed\n");
			}

			if (keys[SDLK_a]){
				printf("LEFT pressed.\n");
			}
			else if (keys[SDLK_d]){
				printf("RIGHT pressed\n");
			}
			*/
		}
	}
}

void updateJoypadState(struct gameboy * gameboy, Uint8 * keys)
{

	for (int i = 0; i < NO_OF_BUTTONS; i++){
		struct buttonMap currentButton = buttons[i];
		printf("%d ", gameboy->joypad.state[i]);
		gameboy->joypad.state[currentButton.button] = keys[currentButton.sdlKey];
	}

	printf("\n");


}

