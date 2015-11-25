#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL/SDL.h>

#define DISPLAY_X 255
#define DISPLAY_Y 255
#define DISPLAY_BPP 0

struct gameboy;

void startDisplay();
void renderGraphics(struct gameboy * gameboy);

#endif
