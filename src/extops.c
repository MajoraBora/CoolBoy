#include "../include/extops.h"
#include "../include/gameboy.h"
#include "../include/flags.h"
#include "../include/bitUtils.h"
#include <stdio.h>

static void cb_rlc(struct gameboy * gameboy, uint8_t * value);
static void cb_rrc(struct gameboy * gameboy, uint8_t * value);
static void cb_rl(struct gameboy * gameboy, uint8_t * value);
static void cb_rr(struct gameboy * gameboy, uint8_t * value);
static void cb_sla(struct gameboy * gameboy, uint8_t * value);
static void cb_sra(struct gameboy * gameboy, uint8_t * value);
static void cb_swap(struct gameboy * gameboy, uint8_t * value);
static void cb_srl(struct gameboy * gameboy, uint8_t * value);
//test bit (bit) in register (reg)
static void cb_testBit(struct gameboy * gameboy, uint8_t bit, uint8_t reg);
static void cb_resetBit(struct gameboy * gameboy, uint8_t bit, uint8_t * reg);
static void cb_setBit(struct gameboy * gameboy, uint8_t bit, uint8_t * reg);


const struct extendedInstruction extendedInstructions[NO_OF_EXT_INSTRUCTIONS] = {
	{ "RLC B", cb_rlc_b, 8},           // 0x00
	{ "RLC C", cb_rlc_c, 8},           // 0x01
	{ "RLC D", cb_rlc_d, 8},           // 0x02
	{ "RLC E", cb_rlc_e, 8},           // 0x03
	{ "RLC H", cb_rlc_h, 8},           // 0x04
	{ "RLC L", cb_rlc_l, 8},           // 0x05
	{ "RLC (HL)", cb_rlc_hlp, 16},      // 0x06
	{ "RLC A", cb_rlc_a, 8},           // 0x07
	{ "RRC B", cb_rrc_b, 8},           // 0x08
	{ "RRC C", cb_rrc_c, 8},           // 0x09
	{ "RRC D", cb_rrc_d, 8},           // 0x0a
	{ "RRC E", cb_rrc_e, 8},           // 0x0b
	{ "RRC H", cb_rrc_h, 8},           // 0x0c
	{ "RRC L", cb_rrc_l, 8},           // 0x0d
	{ "RRC (HL)", cb_rrc_hlp, 16},      // 0x0e
	{ "RRC A", cb_rrc_a, 8},           // 0x0f
	{ "RL B", cb_rl_b, 8},             // 0x10
	{ "RL C", cb_rl_c, 8},             // 0x11
	{ "RL D", cb_rl_d, 8},             // 0x12
	{ "RL E", cb_rl_e, 8},             // 0x13
	{ "RL H", cb_rl_h, 8},             // 0x14
	{ "RL L", cb_rl_l, 8},             // 0x15
	{ "RL (HL)", cb_rl_hlp, 16},        // 0x16
	{ "RL A", cb_rl_a, 8},             // 0x17
	{ "RR B", cb_rr_b, 8},             // 0x18
	{ "RR C", cb_rr_c, 8},             // 0x19
	{ "RR D", cb_rr_d, 8},             // 0x1a
	{ "RR E", cb_rr_e, 8},             // 0x1b
	{ "RR H", cb_rr_h, 8},             // 0x1c
	{ "RR L", cb_rr_l, 8},             // 0x1d
	{ "RR (HL)", cb_rr_hlp, 16},        // 0x1e
	{ "RR A", cb_rr_a, 8},             // 0x1f
	{ "SLA B", cb_sla_b, 8},           // 0x20
	{ "SLA C", cb_sla_c, 8},           // 0x21
	{ "SLA D", cb_sla_d, 8},           // 0x22
	{ "SLA E", cb_sla_e, 8},           // 0x23
	{ "SLA H", cb_sla_h, 8},           // 0x24
	{ "SLA L", cb_sla_l, 8},           // 0x25
	{ "SLA (HL)", cb_sla_hlp, 16},      // 0x26
	{ "SLA A", cb_sla_a, 8},           // 0x27
	{ "SRA B", cb_sra_b, 8},           // 0x28
	{ "SRA C", cb_sra_c, 8},           // 0x29
	{ "SRA D", cb_sra_d, 8},           // 0x2a
	{ "SRA E", cb_sra_e, 8},           // 0x2b
	{ "SRA H", cb_sra_h, 8},           // 0x2c
	{ "SRA L", cb_sra_l, 8},           // 0x2d
	{ "SRA (HL)", cb_sra_hlp, 16},      // 0x2e
	{ "SRA A", cb_sra_a, 8},           // 0x2f
	{ "SWAP B", cb_swap_b, 8},         // 0x30
	{ "SWAP C", cb_swap_c, 8},         // 0x31
	{ "SWAP D", cb_swap_d, 8},         // 0x32
	{ "SWAP E", cb_swap_e, 8},         // 0x33
	{ "SWAP H", cb_swap_h, 8},         // 0x34
	{ "SWAP L", cb_swap_l, 8},         // 0x35
	{ "SWAP (HL)", cb_swap_hlp, 16},    // 0x36
	{ "SWAP A", cb_swap_a, 8},         // 0x37
	{ "SRL B", cb_srl_b, 8},           // 0x38
	{ "SRL C", cb_srl_c, 8},           // 0x39
	{ "SRL D", cb_srl_d, 8},           // 0x3a
	{ "SRL E", cb_srl_e, 8},           // 0x3b
	{ "SRL H", cb_srl_h, 8},           // 0x3c
	{ "SRL L", cb_srl_l, 8},           // 0x3d
	{ "SRL (HL)", cb_srl_hlp, 16},      // 0x3e
	{ "SRL A", cb_srl_a, 8},           // 0x3f
	{ "BIT 0, B", cb_bit_0_b, 8},      // 0x40
	{ "BIT 0, C", cb_bit_0_c, 8},      // 0x41
	{ "BIT 0, D", cb_bit_0_d, 8},      // 0x42
	{ "BIT 0, E", cb_bit_0_e, 8},      // 0x43
	{ "BIT 0, H", cb_bit_0_h, 8},      // 0x44
	{ "BIT 0, L", cb_bit_0_l, 8},      // 0x45
	{ "BIT 0, (HL)", cb_bit_0_hlp, 12}, // 0x46
	{ "BIT 0, A", cb_bit_0_a, 8},      // 0x47
	{ "BIT 1, B", cb_bit_1_b, 8},      // 0x48
	{ "BIT 1, C", cb_bit_1_c, 8},      // 0x49
	{ "BIT 1, D", cb_bit_1_d, 8},      // 0x4a
	{ "BIT 1, E", cb_bit_1_e, 8},      // 0x4b
	{ "BIT 1, H", cb_bit_1_h, 8},      // 0x4c
	{ "BIT 1, L", cb_bit_1_l, 8},      // 0x4d
	{ "BIT 1, (HL)", cb_bit_1_hlp, 12}, // 0x4e
	{ "BIT 1, A", cb_bit_1_a, 8},      // 0x4f
	{ "BIT 2, B", cb_bit_2_b, 8},      // 0x50
	{ "BIT 2, C", cb_bit_2_c, 8},      // 0x51
	{ "BIT 2, D", cb_bit_2_d, 8},      // 0x52
	{ "BIT 2, E", cb_bit_2_e, 8},      // 0x53
	{ "BIT 2, H", cb_bit_2_h, 8},      // 0x54
	{ "BIT 2, L", cb_bit_2_l, 8},      // 0x55
	{ "BIT 2, (HL)", cb_bit_2_hlp, 12}, // 0x56
	{ "BIT 2, A", cb_bit_2_a, 8},      // 0x57
	{ "BIT 3, B", cb_bit_3_b, 8},      // 0x58
	{ "BIT 3, C", cb_bit_3_c, 8},      // 0x59
	{ "BIT 3, D", cb_bit_3_d, 8},      // 0x5a
	{ "BIT 3, E", cb_bit_3_e, 8},      // 0x5b
	{ "BIT 3, H", cb_bit_3_h, 8},      // 0x5c
	{ "BIT 3, L", cb_bit_3_l, 8},      // 0x5d
	{ "BIT 3, (HL)", cb_bit_3_hlp, 12}, // 0x5e
	{ "BIT 3, A", cb_bit_3_a, 8},      // 0x5f
	{ "BIT 4, B", cb_bit_4_b, 8},      // 0x60
	{ "BIT 4, C", cb_bit_4_c, 8},      // 0x61
	{ "BIT 4, D", cb_bit_4_d, 8},      // 0x62
	{ "BIT 4, E", cb_bit_4_e, 8},      // 0x63
	{ "BIT 4, H", cb_bit_4_h, 8},      // 0x64
	{ "BIT 4, L", cb_bit_4_l, 8},      // 0x65
	{ "BIT 4, (HL)", cb_bit_4_hlp, 12}, // 0x66
	{ "BIT 4, A", cb_bit_4_a, 8},      // 0x67
	{ "BIT 5, B", cb_bit_5_b, 8},      // 0x68
	{ "BIT 5, C", cb_bit_5_c, 8},      // 0x69
	{ "BIT 5, D", cb_bit_5_d, 8},      // 0x6a
	{ "BIT 5, E", cb_bit_5_e, 8},      // 0x6b
	{ "BIT 6, H", cb_bit_5_h, 8},      // 0x6c
	{ "BIT 6, L", cb_bit_5_l, 8},      // 0x6d
	{ "BIT 5, (HL)", cb_bit_5_hlp, 12}, // 0x6e
	{ "BIT 5, A", cb_bit_5_a, 8},      // 0x6f
	{ "BIT 6, B", cb_bit_6_b, 8},      // 0x70
	{ "BIT 6, C", cb_bit_6_c, 8},      // 0x71
	{ "BIT 6, D", cb_bit_6_d, 8},      // 0x72
	{ "BIT 6, E", cb_bit_6_e, 8},      // 0x73
	{ "BIT 6, H", cb_bit_6_h, 8},      // 0x74
	{ "BIT 6, L", cb_bit_6_l, 8},      // 0x75
	{ "BIT 6, (HL)", cb_bit_6_hlp, 12}, // 0x76
	{ "BIT 6, A", cb_bit_6_a, 8},      // 0x77
	{ "BIT 7, B", cb_bit_7_b, 8},      // 0x78
	{ "BIT 7, C", cb_bit_7_c, 8},      // 0x79
	{ "BIT 7, D", cb_bit_7_d, 8},      // 0x7a
	{ "BIT 7, E", cb_bit_7_e, 8},      // 0x7b
	{ "BIT 7, H", cb_bit_7_h, 8},      // 0x7c
	{ "BIT 7, L", cb_bit_7_l, 8},      // 0x7d
	{ "BIT 7, (HL)", cb_bit_7_hlp, 12}, // 0x7e
	{ "BIT 7, A", cb_bit_7_a, 8},      // 0x7f
	{ "RES 0, B", cb_res_0_b, 8},      // 0x80
	{ "RES 0, C", cb_res_0_c, 8},      // 0x81
	{ "RES 0, D", cb_res_0_d, 8},      // 0x82
	{ "RES 0, E", cb_res_0_e, 8},      // 0x83
	{ "RES 0, H", cb_res_0_h, 8},      // 0x84
	{ "RES 0, L", cb_res_0_l, 8},      // 0x85
	{ "RES 0, (HL)", cb_res_0_hlp, 12}, // 0x86
	{ "RES 0, A", cb_res_0_a, 8},      // 0x87
	{ "RES 1, B", cb_res_1_b, 8},      // 0x88
	{ "RES 1, C", cb_res_1_c, 8},      // 0x89
	{ "RES 1, D", cb_res_1_d, 8},      // 0x8a
	{ "RES 1, E", cb_res_1_e, 8},      // 0x8b
	{ "RES 1, H", cb_res_1_h, 8},      // 0x8c
	{ "RES 1, L", cb_res_1_l, 8},      // 0x8d
	{ "RES 1, (HL)", cb_res_1_hlp, 12}, // 0x8e
	{ "RES 1, A", cb_res_1_a, 8},      // 0x8f
	{ "RES 2, B", cb_res_2_b, 8},      // 0x90
	{ "RES 2, C", cb_res_2_c, 8},      // 0x91
	{ "RES 2, D", cb_res_2_d, 8},      // 0x92
	{ "RES 2, E", cb_res_2_e, 8},      // 0x93
	{ "RES 2, H", cb_res_2_h, 8},      // 0x94
	{ "RES 2, L", cb_res_2_l, 8},      // 0x95
	{ "RES 2, (HL)", cb_res_2_hlp, 12}, // 0x96
	{ "RES 2, A", cb_res_2_a, 8},      // 0x97
	{ "RES 3, B", cb_res_3_b, 8},      // 0x98
	{ "RES 3, C", cb_res_3_c, 8},      // 0x99
	{ "RES 3, D", cb_res_3_d, 8},      // 0x9a
	{ "RES 3, E", cb_res_3_e, 8},      // 0x9b
	{ "RES 3, H", cb_res_3_h, 8},      // 0x9c
	{ "RES 3, L", cb_res_3_l, 8},      // 0x9d
	{ "RES 3, (HL)", cb_res_3_hlp, 12}, // 0x9e
	{ "RES 3, A", cb_res_3_a, 8},      // 0x9f
	{ "RES 4, B", cb_res_4_b, 8},      // 0xa0
	{ "RES 4, C", cb_res_4_c, 8},      // 0xa1
	{ "RES 4, D", cb_res_4_d, 8},      // 0xa2
	{ "RES 4, E", cb_res_4_e, 8},      // 0xa3
	{ "RES 4, H", cb_res_4_h, 8},      // 0xa4
	{ "RES 4, L", cb_res_4_l, 8},      // 0xa5
	{ "RES 4, (HL)", cb_res_4_hlp, 12}, // 0xa6
	{ "RES 4, A", cb_res_4_a, 8},      // 0xa7
	{ "RES 5, B", cb_res_5_b, 8},      // 0xa8
	{ "RES 5, C", cb_res_5_c, 8},      // 0xa9
	{ "RES 5, D", cb_res_5_d, 8},      // 0xaa
	{ "RES 5, E", cb_res_5_e, 8},      // 0xab
	{ "RES 5, H", cb_res_5_h, 8},      // 0xac
	{ "RES 5, L", cb_res_5_l, 8},      // 0xad
	{ "RES 5, (HL)", cb_res_5_hlp, 12}, // 0xae
	{ "RES 5, A", cb_res_5_a, 8},      // 0xaf
	{ "RES 6, B", cb_res_6_b, 8},      // 0xb0
	{ "RES 6, C", cb_res_6_c, 8},      // 0xb1
	{ "RES 6, D", cb_res_6_d, 8},      // 0xb2
	{ "RES 6, E", cb_res_6_e, 8},      // 0xb3
	{ "RES 6, H", cb_res_6_h, 8},      // 0xb4
	{ "RES 6, L", cb_res_6_l, 8},      // 0xb5
	{ "RES 6, (HL)", cb_res_6_hlp, 12}, // 0xb6
	{ "RES 6, A", cb_res_6_a, 8},      // 0xb7
	{ "RES 7, B", cb_res_7_b, 8},      // 0xb8
	{ "RES 7, C", cb_res_7_c, 8},      // 0xb9
	{ "RES 7, D", cb_res_7_d, 8},      // 0xba
	{ "RES 7, E", cb_res_7_e, 8},      // 0xbb
	{ "RES 7, H", cb_res_7_h, 8},      // 0xbc
	{ "RES 7, L", cb_res_7_l, 8},      // 0xbd
	{ "RES 7, (HL)", cb_res_7_hlp, 12}, // 0xbe
	{ "RES 7, A", cb_res_7_a, 8},      // 0xbf
	{ "SET 0, B", cb_set_0_b, 8},      // 0xc0
	{ "SET 0, C", cb_set_0_c, 8},      // 0xc1
	{ "SET 0, D", cb_set_0_d, 8},      // 0xc2
	{ "SET 0, E", cb_set_0_e, 8},      // 0xc3
	{ "SET 0, H", cb_set_0_h, 8},      // 0xc4
	{ "SET 0, L", cb_set_0_l, 8},      // 0xc5
	{ "SET 0, (HL)", cb_set_0_hlp, 12}, // 0xc6
	{ "SET 0, A", cb_set_0_a, 8},      // 0xc7
	{ "SET 1, B", cb_set_1_b, 8},      // 0xc8
	{ "SET 1, C", cb_set_1_c, 8},      // 0xc9
	{ "SET 1, D", cb_set_1_d, 8},      // 0xca
	{ "SET 1, E", cb_set_1_e, 8},      // 0xcb
	{ "SET 1, H", cb_set_1_h, 8},      // 0xcc
	{ "SET 1, L", cb_set_1_l, 8},      // 0xcd
	{ "SET 1, (HL)", cb_set_1_hlp, 12}, // 0xce
	{ "SET 1, A", cb_set_1_a, 8},      // 0xcf
	{ "SET 2, B", cb_set_2_b, 8},      // 0xd0
	{ "SET 2, C", cb_set_2_c, 8},      // 0xd1
	{ "SET 2, D", cb_set_2_d, 8},      // 0xd2
	{ "SET 2, E", cb_set_2_e, 8},      // 0xd3
	{ "SET 2, H", cb_set_2_h, 8},      // 0xd4
	{ "SET 2, L", cb_set_2_l, 8},      // 0xd5
	{ "SET 2, (HL)", cb_set_2_hlp, 12}, // 0xd6
	{ "SET 2, A", cb_set_2_a, 8},      // 0xd7
	{ "SET 3, B", cb_set_3_b, 8},      // 0xd8
	{ "SET 3, C", cb_set_3_c, 8},      // 0xd9
	{ "SET 3, D", cb_set_3_d, 8},      // 0xda
	{ "SET 3, E", cb_set_3_e, 8},      // 0xdb
	{ "SET 3, H", cb_set_3_h, 8},      // 0xdc
	{ "SET 3, L", cb_set_3_l, 8},      // 0xdd
	{ "SET 3, (HL)", cb_set_3_hlp, 12}, // 0xde
	{ "SET 3, A", cb_set_3_a, 8},      // 0xdf
	{ "SET 4, B", cb_set_4_b, 8},      // 0xe0
	{ "SET 4, C", cb_set_4_c, 8},      // 0xe1
	{ "SET 4, D", cb_set_4_d, 8},      // 0xe2
	{ "SET 4, E", cb_set_4_e, 8},      // 0xe3
	{ "SET 4, H", cb_set_4_h, 8},      // 0xe4
	{ "SET 4, L", cb_set_4_l, 8},      // 0xe5
	{ "SET 4, (HL)", cb_set_4_hlp, 12}, // 0xe6
	{ "SET 4, A", cb_set_4_a, 8},      // 0xe7
	{ "SET 5, B", cb_set_5_b, 8},      // 0xe8
	{ "SET 5, C", cb_set_5_c, 8},      // 0xe9
	{ "SET 5, D", cb_set_5_d, 8},      // 0xea
	{ "SET 5, E", cb_set_5_e, 8},      // 0xeb
	{ "SET 5, H", cb_set_5_h, 8},      // 0xec
	{ "SET 5, L", cb_set_5_l, 8},      // 0xed
	{ "SET 5, (HL)", cb_set_5_hlp, 12}, // 0xee
	{ "SET 5, A", cb_set_5_a, 8},      // 0xef
	{ "SET 6, B", cb_set_6_b, 8},      // 0xf0
	{ "SET 6, C", cb_set_6_c, 8},      // 0xf1
	{ "SET 6, D", cb_set_6_d, 8},      // 0xf2
	{ "SET 6, E", cb_set_6_e, 8},      // 0xf3
	{ "SET 6, H", cb_set_6_h, 8},      // 0xf4
	{ "SET 6, L", cb_set_6_l, 8},      // 0xf5
	{ "SET 6, (HL)", cb_set_6_hlp, 12}, // 0xf6
	{ "SET 6, A", cb_set_6_a, 8},      // 0xf7
	{ "SET 7, B", cb_set_7_b, 8},      // 0xf8
	{ "SET 7, C", cb_set_7_c, 8},      // 0xf9
	{ "SET 7, D", cb_set_7_d, 8},      // 0xfa
	{ "SET 7, E", cb_set_7_e, 8},      // 0xfb
	{ "SET 7, H", cb_set_7_h, 8},      // 0xfc
	{ "SET 7, L", cb_set_7_l, 8},      // 0xfd
	{ "SET 7, (HL)", cb_set_7_hlp, 12}, // 0xfe
	{ "SET 7, A", cb_set_7_a }      // 0xff
};

static void cb_rlc(struct gameboy * gameboy, uint8_t * value)
{
	int carryBit = ((*value) & 0x80) >> 7;
	if ((*value) & 0x80){
		setFlag(gameboy, CARRY, true);
	}
	else {
		setFlag(gameboy, CARRY, false);
	}

	(*value) <<= 1;
	(*value) += carryBit;

	if (*value){
		setFlag(gameboy, ZERO, false);
	}
	else {
		setFlag(gameboy, ZERO, true);
	}

	setFlag(gameboy, SUB, false);
	setFlag(gameboy, HALF_CARRY, false);

}

static void cb_rrc(struct gameboy * gameboy, uint8_t * value)
{
	int carryBit = *value & 0x01;
	
	(*value) >>= 1;	

	if (carryBit){
		setFlag(gameboy, CARRY, true);
		(*value) |= 0x80;
	}
	else {
		setFlag(gameboy, CARRY, false);
	}

	if (*value){
		setFlag(gameboy, ZERO, false);
	}
	else {
		setFlag(gameboy, ZERO, true);
	}

	setFlag(gameboy, SUB, false);
	setFlag(gameboy, HALF_CARRY, false);

}

static void cb_rl(struct gameboy * gameboy, uint8_t * value)
{
	int carryBit = isFlagSet(gameboy, CARRY);
	
	if ((*value) & 0x80){
		setFlag(gameboy, CARRY, true);
	}
	else {
		setFlag(gameboy, CARRY, false);
	}

	(*value) <<= 1;
	(*value) += carryBit;

	if (*value){
		setFlag(gameboy, ZERO, false);
	}
	else {
		setFlag(gameboy, ZERO, true);
	}

	setFlag(gameboy, SUB, false);
	setFlag(gameboy, HALF_CARRY, false);

}

static void cb_rr(struct gameboy * gameboy, uint8_t * value)
{
	(*value) >>= 1;
	if (isFlagSet(gameboy, CARRY)){
		(*value) |= 0x80;
	}

	if ((*value) & 0x01){
		setFlag(gameboy, CARRY, true);
	}
	else {
		setFlag(gameboy, CARRY, false);
	}

	if (*value){
		setFlag(gameboy, ZERO, false);
	}
	else {
		setFlag(gameboy, ZERO, true);
	}

	setFlag(gameboy, SUB, false);
	setFlag(gameboy, HALF_CARRY, false);

}

static void cb_sla(struct gameboy * gameboy, uint8_t * value)
{
	if ((*value) & 0x80){
		setFlag(gameboy, CARRY, true);
	}
	else {
		setFlag(gameboy, CARRY, false);
	}

	(*value) <<= 1;

	if (*value){
		setFlag(gameboy, ZERO, false);
	}
	else {
		setFlag(gameboy, ZERO, true);
	}

	setFlag(gameboy, SUB, false);
	setFlag(gameboy, HALF_CARRY, false);

}

static void cb_sra(struct gameboy * gameboy, uint8_t * value)
{
	if ((*value) & 0x01){
		setFlag(gameboy, CARRY, true);
	}
	else {
		setFlag(gameboy, CARRY, false);
	}

	uint8_t newValue = (((*value) & 0x80) | ((*value) >> 1));
	*value = newValue;

	if (*value){
		setFlag(gameboy, ZERO, false);
	}
	else {
		setFlag(gameboy, ZERO, true);
	}
	
	setFlag(gameboy, SUB, false);
	setFlag(gameboy, HALF_CARRY, false);
	
}

static void cb_swap(struct gameboy * gameboy, uint8_t * value)
{
	uint8_t swapped = (((*value) & 0xF) << 4) | (((*value) & 0xF) >> 4);
	*value = swapped;

	if (*value){
		setFlag(gameboy, ZERO, false);
	}
	else {
		setFlag(gameboy, ZERO, true);
	}

	setFlag(gameboy, SUB, false);
	setFlag(gameboy, HALF_CARRY, false);
	setFlag(gameboy, CARRY, false);
}

static void cb_srl(struct gameboy * gameboy, uint8_t * value)
{
	if ((*value) & 0x01){
		setFlag(gameboy, CARRY, true);
	}
	else {
		setFlag(gameboy, CARRY, false);
	}
	
	(*value) >>= 1;

	if (*value){
		setFlag(gameboy, ZERO, false);
	}
	else {
		setFlag(gameboy, ZERO, true);
	}

	setFlag(gameboy, SUB, false);
	setFlag(gameboy, HALF_CARRY, false);

}

//test bit (bit) in register (reg)
static void cb_testBit(struct gameboy * gameboy, uint8_t bit, uint8_t reg)
{
	if (isBitSet(reg, bit)){
		setFlag(gameboy, ZERO, false);
	}
	else {
		setFlag(gameboy, ZERO, true);
	}

	setFlag(gameboy, SUB, false);
	setFlag(gameboy, HALF_CARRY, true);
}

static void cb_resetBit(struct gameboy * gameboy, uint8_t bit, uint8_t * reg)
{
	setBit(reg, bit, false);
}

static void cb_setBit(struct gameboy * gameboy, uint8_t bit, uint8_t * reg)
{
	setBit(reg, bit, true);
}

void executeExtendedOpcode(struct gameboy * gameboy, uint8_t opcode)
{
	struct extendedInstruction instruction = extendedInstructions[opcode];
	uint8_t cycles = instruction.cycles;
	
	((void(*)(struct gameboy *))instruction.function)(gameboy); //do instruction

	gameboy->cpu.cycles += cycles;

//	printf("extended op: %s\n", instruction.instruction);
}

void cb_rlc_b(struct gameboy * gameboy)
{
	cb_rlc(gameboy, &gameboy->cpu.b);
}

void cb_rlc_c(struct gameboy * gameboy)
{
	cb_rlc(gameboy, &gameboy->cpu.c);
}

void cb_rlc_d(struct gameboy * gameboy)
{
	cb_rlc(gameboy, &gameboy->cpu.d);

}

void cb_rlc_e(struct gameboy * gameboy)
{
	cb_rlc(gameboy, &gameboy->cpu.e);

}

void cb_rlc_h(struct gameboy * gameboy)
{
	cb_rlc(gameboy, &gameboy->cpu.h);

}

void cb_rlc_l(struct gameboy * gameboy)
{
	cb_rlc(gameboy, &gameboy->cpu.l);

}

void cb_rlc_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_rlc(gameboy, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_rlc_a(struct gameboy * gameboy)
{
	cb_rlc(gameboy, &gameboy->cpu.a);

}

void cb_rrc_b(struct gameboy * gameboy)
{
	cb_rrc(gameboy, &gameboy->cpu.b);
}

void cb_rrc_c(struct gameboy * gameboy)
{
	cb_rrc(gameboy, &gameboy->cpu.c);

}

void cb_rrc_d(struct gameboy * gameboy)
{
	cb_rrc(gameboy, &gameboy->cpu.d);

}

void cb_rrc_e(struct gameboy * gameboy)
{
	cb_rrc(gameboy, &gameboy->cpu.e);

}

void cb_rrc_h(struct gameboy * gameboy)
{
	cb_rrc(gameboy, &gameboy->cpu.h);

}

void cb_rrc_l(struct gameboy * gameboy)
{
	cb_rrc(gameboy, &gameboy->cpu.l);

}

void cb_rrc_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_rrc(gameboy, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_rrc_a(struct gameboy * gameboy)
{
	cb_rrc(gameboy, &gameboy->cpu.a);

}

void cb_rl_b(struct gameboy * gameboy)
{
	cb_rl(gameboy, &gameboy->cpu.b);

}

void cb_rl_c(struct gameboy * gameboy)
{
	cb_rl(gameboy, &gameboy->cpu.c);
}

void cb_rl_d(struct gameboy * gameboy)
{
	cb_rl(gameboy, &gameboy->cpu.d);

}

void cb_rl_e(struct gameboy * gameboy)
{
	cb_rl(gameboy, &gameboy->cpu.e);

}

void cb_rl_h(struct gameboy * gameboy)
{
	cb_rl(gameboy, &gameboy->cpu.h);

}

void cb_rl_l(struct gameboy * gameboy)
{
	cb_rl(gameboy, &gameboy->cpu.l);

}

void cb_rl_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_rl(gameboy, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_rl_a(struct gameboy * gameboy)
{
	cb_rl(gameboy, &gameboy->cpu.a);

}

void cb_rr_b(struct gameboy * gameboy)
{
	cb_rr(gameboy, &gameboy->cpu.b);

}

void cb_rr_c(struct gameboy * gameboy)
{
	cb_rr(gameboy, &gameboy->cpu.c);

}

void cb_rr_d(struct gameboy * gameboy)
{
	cb_rr(gameboy, &gameboy->cpu.d);

}

void cb_rr_e(struct gameboy * gameboy)
{
	cb_rr(gameboy, &gameboy->cpu.e);

}

void cb_rr_h(struct gameboy * gameboy)
{
	cb_rr(gameboy, &gameboy->cpu.h);

}

void cb_rr_l(struct gameboy * gameboy)
{
	cb_rr(gameboy, &gameboy->cpu.l);

}

void cb_rr_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_rr(gameboy, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_rr_a(struct gameboy * gameboy)
{
	cb_rr(gameboy, &gameboy->cpu.a);

}

void cb_sla_b(struct gameboy * gameboy)
{
	cb_sla(gameboy, &gameboy->cpu.b);

}

void cb_sla_c(struct gameboy * gameboy)
{
	cb_sla(gameboy, &gameboy->cpu.c);

}

void cb_sla_d(struct gameboy * gameboy)
{
	cb_sla(gameboy, &gameboy->cpu.d);

}

void cb_sla_e(struct gameboy * gameboy)
{
	cb_sla(gameboy, &gameboy->cpu.e);

}

void cb_sla_h(struct gameboy * gameboy)
{

	cb_sla(gameboy, &gameboy->cpu.h);
}

void cb_sla_l(struct gameboy * gameboy)
{
	cb_sla(gameboy, &gameboy->cpu.l);

}

void cb_sla_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_sla(gameboy, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_sla_a(struct gameboy * gameboy)
{
	cb_sla(gameboy, &gameboy->cpu.a);

}

void cb_sra_b(struct gameboy * gameboy)
{
	cb_sra(gameboy, &gameboy->cpu.b);

}

void cb_sra_c(struct gameboy * gameboy)
{
	cb_sra(gameboy, &gameboy->cpu.c);

}

void cb_sra_d(struct gameboy * gameboy)
{
	cb_sra(gameboy, &gameboy->cpu.d);

}

void cb_sra_e(struct gameboy * gameboy)
{
	cb_sra(gameboy, &gameboy->cpu.e);

}

void cb_sra_h(struct gameboy * gameboy)
{
	cb_sra(gameboy, &gameboy->cpu.h);

}

void cb_sra_l(struct gameboy * gameboy)
{
	cb_sra(gameboy, &gameboy->cpu.l);

}

void cb_sra_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_sra(gameboy, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_sra_a(struct gameboy * gameboy)
{
	cb_sra(gameboy, &gameboy->cpu.a);

}

void cb_swap_b(struct gameboy * gameboy)
{
	cb_swap(gameboy, &gameboy->cpu.b);

}

void cb_swap_c(struct gameboy * gameboy)
{
	cb_swap(gameboy, &gameboy->cpu.c);

}

void cb_swap_d(struct gameboy * gameboy)
{
	cb_swap(gameboy, &gameboy->cpu.d);

}

void cb_swap_e(struct gameboy * gameboy)
{
	cb_swap(gameboy, &gameboy->cpu.e);

}

void cb_swap_h(struct gameboy * gameboy)
{
	cb_swap(gameboy, &gameboy->cpu.h);

}

void cb_swap_l(struct gameboy * gameboy)
{
	cb_swap(gameboy, &gameboy->cpu.l);

}

void cb_swap_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_swap(gameboy, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_swap_a(struct gameboy * gameboy)
{
	cb_swap(gameboy, &gameboy->cpu.a);

}

void cb_srl_b(struct gameboy * gameboy)
{
	cb_srl(gameboy, &gameboy->cpu.b);

}

void cb_srl_c(struct gameboy * gameboy)
{
	cb_srl(gameboy, &gameboy->cpu.c);

}

void cb_srl_d(struct gameboy * gameboy)
{
	cb_srl(gameboy, &gameboy->cpu.d);

}

void cb_srl_e(struct gameboy * gameboy)
{
	cb_srl(gameboy, &gameboy->cpu.e);

}

void cb_srl_h(struct gameboy * gameboy)
{
	cb_srl(gameboy, &gameboy->cpu.h);

}

void cb_srl_l(struct gameboy * gameboy)
{
	cb_srl(gameboy, &gameboy->cpu.l);

}

void cb_srl_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_srl(gameboy, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_srl_a(struct gameboy * gameboy)
{
	cb_srl(gameboy, &gameboy->cpu.a);

}

void cb_bit_0_b(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 0, gameboy->cpu.b);
}

void cb_bit_0_c(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 0, gameboy->cpu.c);

}

void cb_bit_0_d(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 0, gameboy->cpu.d);

}

void cb_bit_0_e(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 0, gameboy->cpu.e);

}

void cb_bit_0_h(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 0, gameboy->cpu.h);

}

void cb_bit_0_l(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 0, gameboy->cpu.l);

}

void cb_bit_0_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_testBit(gameboy, 0, byte);

}

void cb_bit_0_a(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 0, gameboy->cpu.a);

}

void cb_bit_1_b(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 1, gameboy->cpu.b);

}

void cb_bit_1_c(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 1, gameboy->cpu.c);

}

void cb_bit_1_d(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 1, gameboy->cpu.d);

}

void cb_bit_1_e(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 1, gameboy->cpu.e);

}

void cb_bit_1_h(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 1, gameboy->cpu.h);

}

void cb_bit_1_l(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 1, gameboy->cpu.l);

}

void cb_bit_1_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_testBit(gameboy, 1, byte);

}

void cb_bit_1_a(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 1, gameboy->cpu.a);

}

void cb_bit_2_b(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 2, gameboy->cpu.b);

}

void cb_bit_2_c(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 2, gameboy->cpu.c);

}

void cb_bit_2_d(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 2, gameboy->cpu.d);

}

void cb_bit_2_e(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 2, gameboy->cpu.e);

}

void cb_bit_2_h(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 2, gameboy->cpu.h);

}

void cb_bit_2_l(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 2, gameboy->cpu.l);

}

void cb_bit_2_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_testBit(gameboy, 2, byte);

}

void cb_bit_2_a(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 2, gameboy->cpu.a);

}

void cb_bit_3_b(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 3, gameboy->cpu.b);

}

void cb_bit_3_c(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 3, gameboy->cpu.c);

}

void cb_bit_3_d(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 3, gameboy->cpu.d);

}

void cb_bit_3_e(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 3, gameboy->cpu.e);

}

void cb_bit_3_h(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 3, gameboy->cpu.h);

}

void cb_bit_3_l(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 3, gameboy->cpu.l);

}

void cb_bit_3_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_testBit(gameboy, 3, byte);

}

void cb_bit_3_a(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 3, gameboy->cpu.a);

}

void cb_bit_4_b(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 4, gameboy->cpu.b);

}

void cb_bit_4_c(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 4, gameboy->cpu.c);

}

void cb_bit_4_d(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 4, gameboy->cpu.d);

}

void cb_bit_4_e(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 4, gameboy->cpu.e);

}

void cb_bit_4_h(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 4, gameboy->cpu.h);

}

void cb_bit_4_l(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 4, gameboy->cpu.l);

}

void cb_bit_4_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_testBit(gameboy, 4, byte);

}

void cb_bit_4_a(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 4, gameboy->cpu.a);

}

void cb_bit_5_b(struct gameboy * gameboy)
{

	cb_testBit(gameboy, 5, gameboy->cpu.b);
}

void cb_bit_5_c(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 5, gameboy->cpu.c);

}

void cb_bit_5_d(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 5, gameboy->cpu.d);

}

void cb_bit_5_e(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 5, gameboy->cpu.e);

}

void cb_bit_5_h(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 5, gameboy->cpu.h);

}

void cb_bit_5_l(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 5, gameboy->cpu.l);

}

void cb_bit_5_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_testBit(gameboy, 5, byte);

}

void cb_bit_5_a(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 5, gameboy->cpu.a);

}

void cb_bit_6_b(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 6, gameboy->cpu.b);

}

void cb_bit_6_c(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 6, gameboy->cpu.c);

}

void cb_bit_6_d(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 6, gameboy->cpu.d);

}

void cb_bit_6_e(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 6, gameboy->cpu.e);

}

void cb_bit_6_h(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 6, gameboy->cpu.h);

}

void cb_bit_6_l(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 6, gameboy->cpu.l);

}

void cb_bit_6_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_testBit(gameboy, 6, byte);

}

void cb_bit_6_a(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 6, gameboy->cpu.a);

}

void cb_bit_7_b(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 0, gameboy->cpu.b);

}

void cb_bit_7_c(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 7, gameboy->cpu.c);

}

void cb_bit_7_d(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 7, gameboy->cpu.d);

}

void cb_bit_7_e(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 7, gameboy->cpu.e);

}

void cb_bit_7_h(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 7, gameboy->cpu.h);

}

void cb_bit_7_l(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 7, gameboy->cpu.l);

}

void cb_bit_7_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_testBit(gameboy, 7, byte);

}

void cb_bit_7_a(struct gameboy * gameboy)
{
	cb_testBit(gameboy, 7, gameboy->cpu.a);

}

void cb_res_0_b(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 0, &gameboy->cpu.b);
}

void cb_res_0_c(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 0, &gameboy->cpu.c);

}

void cb_res_0_d(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 0, &gameboy->cpu.d);

}

void cb_res_0_e(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 0, &gameboy->cpu.e);

}

void cb_res_0_h(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 0, &gameboy->cpu.h);

}

void cb_res_0_l(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 0, &gameboy->cpu.l);

}

void cb_res_0_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_resetBit(gameboy, 0, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_res_0_a(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 0, &gameboy->cpu.a);

}

void cb_res_1_b(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 1, &gameboy->cpu.b);

}

void cb_res_1_c(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 1, &gameboy->cpu.c);

}

void cb_res_1_d(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 1, &gameboy->cpu.d);

}

void cb_res_1_e(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 1, &gameboy->cpu.e);

}

void cb_res_1_h(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 1, &gameboy->cpu.h);

}

void cb_res_1_l(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 1, &gameboy->cpu.l);

}

void cb_res_1_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_resetBit(gameboy, 1, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_res_1_a(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 1, &gameboy->cpu.a);

}

void cb_res_2_b(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 2, &gameboy->cpu.b);

}

void cb_res_2_c(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 2, &gameboy->cpu.c);

}

void cb_res_2_d(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 2, &gameboy->cpu.d);

}

void cb_res_2_e(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 2, &gameboy->cpu.e);

}

void cb_res_2_h(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 2, &gameboy->cpu.h);

}

void cb_res_2_l(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 2, &gameboy->cpu.l);

}

void cb_res_2_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_resetBit(gameboy, 2, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_res_2_a(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 2, &gameboy->cpu.a);

}

void cb_res_3_b(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 3, &gameboy->cpu.b);

}

void cb_res_3_c(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 3, &gameboy->cpu.c);

}

void cb_res_3_d(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 3, &gameboy->cpu.d);

}

void cb_res_3_e(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 3, &gameboy->cpu.e);

}

void cb_res_3_h(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 3, &gameboy->cpu.h);

}

void cb_res_3_l(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 3, &gameboy->cpu.l);

}

void cb_res_3_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_resetBit(gameboy, 3, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_res_3_a(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 3, &gameboy->cpu.a);

}

void cb_res_4_b(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 4, &gameboy->cpu.b);

}

void cb_res_4_c(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 4, &gameboy->cpu.c);

}

void cb_res_4_d(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 4, &gameboy->cpu.d);

}

void cb_res_4_e(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 4, &gameboy->cpu.e);

}

void cb_res_4_h(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 4, &gameboy->cpu.h);

}

void cb_res_4_l(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 4, &gameboy->cpu.l);

}

void cb_res_4_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_resetBit(gameboy, 4, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_res_4_a(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 4, &gameboy->cpu.a);

}

void cb_res_5_b(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 5, &gameboy->cpu.b);

}

void cb_res_5_c(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 5, &gameboy->cpu.c);

}

void cb_res_5_d(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 5, &gameboy->cpu.d);

}

void cb_res_5_e(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 5, &gameboy->cpu.e);

}

void cb_res_5_h(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 5, &gameboy->cpu.h);

}

void cb_res_5_l(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 5, &gameboy->cpu.l);

}

void cb_res_5_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_resetBit(gameboy, 5, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_res_5_a(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 5, &gameboy->cpu.a);

}

void cb_res_6_b(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 6, &gameboy->cpu.b);

}

void cb_res_6_c(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 6, &gameboy->cpu.c);

}

void cb_res_6_d(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 6, &gameboy->cpu.d);

}

void cb_res_6_e(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 6, &gameboy->cpu.e);

}

void cb_res_6_h(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 6, &gameboy->cpu.h);

}

void cb_res_6_l(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 6, &gameboy->cpu.l);

}

void cb_res_6_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_resetBit(gameboy, 6, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_res_6_a(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 6, &gameboy->cpu.a);

}

void cb_res_7_b(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 7, &gameboy->cpu.b);

}

void cb_res_7_c(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 7, &gameboy->cpu.c);

}

void cb_res_7_d(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 7, &gameboy->cpu.d);

}

void cb_res_7_e(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 7, &gameboy->cpu.e);

}

void cb_res_7_h(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 7, &gameboy->cpu.h);

}

void cb_res_7_l(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 7, &gameboy->cpu.l);

}

void cb_res_7_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_resetBit(gameboy, 7, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_res_7_a(struct gameboy * gameboy)
{
	cb_resetBit(gameboy, 7, &gameboy->cpu.a);

}

void cb_set_0_b(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 0, &gameboy->cpu.b);
}

void cb_set_0_c(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 0, &gameboy->cpu.c);

}

void cb_set_0_d(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 0, &gameboy->cpu.d);

}

void cb_set_0_e(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 0, &gameboy->cpu.e);

}

void cb_set_0_h(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 0, &gameboy->cpu.h);

}

void cb_set_0_l(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 0, &gameboy->cpu.l);

}

void cb_set_0_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_setBit(gameboy, 0, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_set_0_a(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 0, &gameboy->cpu.a);

}

void cb_set_1_b(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 1, &gameboy->cpu.b);

}

void cb_set_1_c(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 1, &gameboy->cpu.c);

}

void cb_set_1_d(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 1, &gameboy->cpu.d);

}

void cb_set_1_e(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 1, &gameboy->cpu.e);

}

void cb_set_1_h(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 1, &gameboy->cpu.h);

}

void cb_set_1_l(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 1, &gameboy->cpu.l);

}

void cb_set_1_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_setBit(gameboy, 1, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_set_1_a(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 1, &gameboy->cpu.a);

}

void cb_set_2_b(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 2, &gameboy->cpu.b);

}

void cb_set_2_c(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 2, &gameboy->cpu.c);

}

void cb_set_2_d(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 2, &gameboy->cpu.d);

}

void cb_set_2_e(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 2, &gameboy->cpu.e);

}

void cb_set_2_h(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 2, &gameboy->cpu.h);

}

void cb_set_2_l(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 2, &gameboy->cpu.l);

}

void cb_set_2_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_setBit(gameboy, 2, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_set_2_a(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 2, &gameboy->cpu.a);

}

void cb_set_3_b(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 3, &gameboy->cpu.b);

}

void cb_set_3_c(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 3, &gameboy->cpu.c);

}

void cb_set_3_d(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 3, &gameboy->cpu.d);

}

void cb_set_3_e(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 3, &gameboy->cpu.e);

}

void cb_set_3_h(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 3, &gameboy->cpu.h);

}

void cb_set_3_l(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 3, &gameboy->cpu.l);

}

void cb_set_3_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_setBit(gameboy, 3, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_set_3_a(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 3, &gameboy->cpu.a);

}

void cb_set_4_b(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 4, &gameboy->cpu.b);

}

void cb_set_4_c(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 4, &gameboy->cpu.c);

}

void cb_set_4_d(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 4, &gameboy->cpu.d);

}

void cb_set_4_e(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 4, &gameboy->cpu.e);

}

void cb_set_4_h(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 4, &gameboy->cpu.h);

}

void cb_set_4_l(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 4, &gameboy->cpu.l);

}

void cb_set_4_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_setBit(gameboy, 4, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_set_4_a(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 4, &gameboy->cpu.a);

}

void cb_set_5_b(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 5, &gameboy->cpu.b);

}

void cb_set_5_c(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 5, &gameboy->cpu.c);

}

void cb_set_5_d(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 5, &gameboy->cpu.d);

}

void cb_set_5_e(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 5, &gameboy->cpu.e);

}

void cb_set_5_h(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 5, &gameboy->cpu.h);

}

void cb_set_5_l(struct gameboy * gameboy)
{

	cb_setBit(gameboy, 5, &gameboy->cpu.l);
}

void cb_set_5_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_setBit(gameboy, 5, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_set_5_a(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 5, &gameboy->cpu.a);

}

void cb_set_6_b(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 6, &gameboy->cpu.b);

}

void cb_set_6_c(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 6, &gameboy->cpu.c);

}

void cb_set_6_d(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 6, &gameboy->cpu.d);

}

void cb_set_6_e(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 6, &gameboy->cpu.e);

}

void cb_set_6_h(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 6, &gameboy->cpu.h);

}

void cb_set_6_l(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 6, &gameboy->cpu.l);

}

void cb_set_6_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_setBit(gameboy, 6, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_set_6_a(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 6, &gameboy->cpu.a);

}

void cb_set_7_b(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 7, &gameboy->cpu.b);

}

void cb_set_7_c(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 7, &gameboy->cpu.c);

}

void cb_set_7_d(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 7, &gameboy->cpu.d);

}

void cb_set_7_e(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 7, &gameboy->cpu.e);

}

void cb_set_7_h(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 7, &gameboy->cpu.h);

}

void cb_set_7_l(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 7, &gameboy->cpu.l);

}

void cb_set_7_hlp(struct gameboy * gameboy)
{
	uint8_t byte = readByte(gameboy, gameboy->cpu.hl);
	cb_setBit(gameboy, 7, &byte);
	writeByte(gameboy, gameboy->cpu.hl, byte);

}

void cb_set_7_a(struct gameboy * gameboy)
{
	cb_setBit(gameboy, 7, &gameboy->cpu.a);

}

