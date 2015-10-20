#ifndef CARTRIDGE_H
#define CARTRIDGE_H

struct gameboy;

void loadGame(struct gameboy * gameboy, const char * directory);

#endif
