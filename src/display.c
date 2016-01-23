#include "../include/display.h"
#include "../include/gameboy.h"
#include <GL/gl.h>

static void initialiseSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
                fprintf(stderr, "Couldn't initialise SDL.\n");
                exit(-1);
        }

        if (!SDL_SetVideoMode(DISPLAY_X, DISPLAY_Y, DISPLAY_BPP, SDL_OPENGL)){
                fprintf(stderr, "Couldn't initialise SDL Window.\n");
                SDL_Quit();
                exit(-1);
        }
}

static void initialiseOpenGL()
{
	
	glViewport(0, 0, DISPLAY_X, DISPLAY_Y);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0, DISPLAY_X, DISPLAY_Y, 0, -1.0, 1.0);
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_FLAT);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);

}

void startDisplay()
{
	initialiseSDL();
	initialiseOpenGL();
}

void renderGraphics(struct gameboy * gameboy)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glRasterPos2i(-1, 1);
	glPixelZoom(1, -1);
	//glDrawPixels(X, Y, GL_RGB, GL_UNSIGNED_BYTE, gameboy->screen.frameBuffer);
	//glDrawPixels(X, Y, GL_RGB, GL_UNSIGNED_BYTE, gameboy->screen.frameBuffer3D);
	glDrawPixels(X, Y, GL_RGB, GL_UNSIGNED_BYTE, gameboy->screen.frameBufferNew);
	SDL_GL_SwapBuffers();
}

