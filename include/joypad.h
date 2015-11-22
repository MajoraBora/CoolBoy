#ifndef JOYPAD_H
#define JOYPAD_H

#include <SDL/SDL.h>
#include <stdbool.h>

#define JOYPAD_REG 0xFF00
#define JOYPAD_REG_INIT 0xF
#define NO_OF_BUTTONS 8

struct gameboy;

/*
Joypad register is 8 bits
	7 - not used
	6 - not used
	5 - Select Button Keys (0 = Select)
	4 - Select Direction Keys (0 = Select)
	3 - Input Down or Start (read only, 0 = pressed)
	2 - Input Up or Select (...)
	1 - Input Left or Button B (...)
	0 - Input Right or Button A (...)
	 
*/

enum button {
	SELECT,
	START,
	B,
	A,
	RIGHT,
	LEFT,
	DOWN,
	UP
};

struct buttonMap {
	enum button button;
	SDLKey sdlKey;
};

struct joypad {
	uint8_t * state; //0xFF00
	uint8_t * previousState;
	uint8_t reg;
	uint8_t buttonState;
	bool buttonMode; //0 for directions, 1 for buttons
	/*
	bool up;
	bool down;
	bool left;
	bool right;
	bool a;
	bool b;
	bool start;
	bool select;
	*/
};

extern struct buttonMap buttons[NO_OF_BUTTONS];

void updateJoypadState(struct gameboy * gameboy, Uint8 * keys);

/*
void pressUp(struct gameboy * gameboy);
void pressDown(struct gameboy * gameboy);
void pressLeft(struct gameboy * gameboy);
void pressRigt(struct gameboy * gameboy);
void pressA(struct gameboy * gameboy);
void pressB(struct gameboy * gameboy);
void pressStart(struct gameboy * gameboy);
void pressSelect(struct gameboy * gameboy);
*/

/*
Called when SDL key event is received
Send this event to the joypad, which will figure out if it is a valid key
if valid, set correct bits, request any interrupts
*/
void keyPressed(struct gameboy * gameboy, SDL_Event * event);
void handleKeys(SDL_Event * event, int * quit);
void startKeyModule();
#endif
