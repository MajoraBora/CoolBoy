#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdint.h>

#define MBC_MODE_ADDRESS 0x147
#define ROM 0x148
#define EXTERNAL_RAM 0x149
#define DESTINATION 0x14A

enum mbcMode {
        ROM_ONLY = 0x0,
        MBC1 = 0x01,
        MBC1_RAM = 0x02,
        MBC_RAM_BATTERY = 0x03,
        MBC2 = 0x05,
        MBC2_BATTERY = 0x06,
        ROM_RAM = 0x08,
        ROM_RAM_BATTERY = 0x09,
        MMM01 = 0x0B,
        MMM01_RAM = 0x0C,
        MMM01_RAM_BATTERY = 0x0D,
        MBC3_TIMER_RAM_BATTERY = 0x0F,
        MBC3 = 0x11,
        MBC3_RAM = 0x12,
        MBC3_RAM_BATTERY = 0x13,
        MBC4 = 0x15,
        MBC4_RAM = 0x16,
        MBC4_RAM_BATTERY = 0x17,
        MBC5 = 0x19,
        MBC5_RAM = 0x1A,
        MBC5_RAM_BATTERY = 0x1B,
        MBC5_RUMBLE = 0x1C,
        MBC5_RUMBLE_RAM = 0x1D,
        MBC5_RUMBLE_RAM_BATTERY = 0x1E,
        POCKET_CAMERA = 0xFC,
        BANDAI_TAMA5 = 0xFD,
        HUC3 = 0xFE,
        HUC1_RAM_BATTERY = 0xFF
};

struct romInfo {
	float size; //size of ROM in bytes
	uint16_t noOfBanks; //number of ROM banks
};

struct ramInfo {
	float size;
	uint16_t noOfBanks;
};

extern struct romInfo romInfoChoices[0x54];
extern struct ramInfo ramInfoChoices[0x03];
extern char * destinations[0x2];

struct gameboy;

void loadGame(struct gameboy * gameboy, const char * directory);
void loadBankType(struct gameboy * gameboy);
void loadRomInfo(struct gameboy * gameboy);
void loadRamInfo(struct gameboy * gameboy);
void loadDestinationInfo(struct gameboy * gameboy);
void printCartDetails(struct gameboy * gameboy);

#endif
