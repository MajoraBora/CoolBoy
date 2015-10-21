#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#define MBC_MODE_ADDRESS 0x147

struct gameboy;

void loadGame(struct gameboy * gameboy, const char * directory);
void loadBankType(struct gameboy * gameboy);
void printCartDetails(struct gameboy * gameboy);

#endif
