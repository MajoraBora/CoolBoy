#ifndef REGISTER_H
#define REGISTER_H
#include <stdint.h>

#define INIT_PROGRAM_COUNTER 0x100
#define INIT_AF 0x01B0
#define INIT_BC 0x0013
#define INIT_DE 0x00D8
#define INIT_HL 0x014D
#define INIT_STACK_POINTER 0xFFFE


union reg {
	uint16_t value;
	struct {
		uint8_t lo;
		uint8_t hi;
	};
};


#endif
