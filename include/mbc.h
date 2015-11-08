#ifndef MBC_H
#define MBC_H

#define BANK_ZERO_SIZE 0x4000
#define ROM_ONLY_CARTRIDGE_SIZE 0x8000

struct gameboy;

//some initialisation functions - memory layout etc.
void initialiseRomBanks(struct gameboy * gameboy);

#endif
