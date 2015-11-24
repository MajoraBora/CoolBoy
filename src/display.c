#include "../include/display.h"

void startDisplay()
{
        if (SDL_Init(SDL_INIT_VIDEO) < 0){
                fprintf(stderr, "Couldn't initialise SDL.\n");
                exit(-1);
        }

        if (!SDL_SetVideoMode(DISPLAY_X, DISPLAY_Y, DISPLAY_BPP, SDL_HWSURFACE)){
                fprintf(stderr, "Couldn't initialise SDL Window.\n");
                SDL_Quit();
                exit(-1);
        }

        SDL_EnableUNICODE(1);

}

