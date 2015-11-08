#include "../include/cpu.h"
#include "../include/gameboy.h"
#include "../include/stack.h"
#include "../include/interrupt.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

const struct instruction instructions[NO_OF_INSTRUCTIONS] = {
	// 0x00 - 0x0F
	{"NOP", 0, nop, 4},
	{"LD BC NN", 2, ld_bc_nn, 12},
	{"LD (BC), A", 0, ld_bcp_a, 8},
	{"INC BC", 0, inc_bc, 8},
	{"INC B", 0, inc_b, 4},
	{"DEC B", 0, dec_b, 4},
	{"LD B, n", 1, ld_b_n, 8},
	{"RLC A", 0, rlc_a, 8},
	{"LD (nn), SP", 2, ld_nn_sp, 20},
	{"ADD HL, BC", 0, add_hl_bc, 8},
	{"LD A, (BC)", 0, ld_a_bcp, 8},
	{"DEC BC", 0, dec_bc, 8},
	{"INC C", 0, inc_c, 4},
	{"DEC C", 0, dec_c, 4},
	{"LD C, n", 1, ld_c_n, 8},
	{"RRC A", 0, rrc_a, 8},
	
	// 0x10 - 0x1F
	{"STOP", 0, stop, 4},
	{"LD DE", 2, ld_de_nn, 12},
	{"LD (DE), A", 0, ld_dep_a, 8},
	{"INC DE", 0, inc_de, 8},
	{"INC D", 0, inc_d, 4},
	{"DEC D", 0, dec_d, 4},
	{"LD D", 1, ld_d_n, 8},
	{"RLA", 0, rl_a, 8},
	{"JR", 1, jr_n, 8},
	{"ADD HL, DE", 0, add_hl_de, 8},
	{"LD A, (DE)", 0, ld_a_dep, 8},
	{"DEC DE", 0, dec_de, 8},
	{"INC E", 0, inc_e, 4},
	{"DEC E", 0, dec_e, 4},
	{"LD E N", 1, ld_e_n, 8},
	{"RR A", 0, rr_a, 8},
	
	// 0x20 - 0x2F
	{"JR NZ, n", 1, jr_nz_n, 8},
	{"LD HL, nn", 2, ld_hl_nn, 12},
	{"LDI (HL), A", 0, ldi_hlp_a, 8}, //load 16 bit with inc
	{"INC HL", 0, inc_hl, 8},
	{"INC H", 0, inc_h, 4},
	{"DEC H", 0, dec_h, 4},
	{"LD H, n", 1, ld_h_n, 8},
	{"DAA", 0, daa, 4},
	{"JR Z, n", 1, jr_z_n, 8},
	{"ADD HL, HL", 0, add_hl_hl, 8},
	{"LDIeA, (HL)", 0, ldi_a_hlp, 8},
	{"DEC HL", 0, dec_hl, 8},
	{"INC L", 0, inc_h, 4},
	{"DEC L", 0, dec_h, 4},
	{"LD L", 1, ld_l_n, 8},
	{"CPL", 0, cpl, 4},

	// 0x30 - 0x3F
	{"JR NC, n", 1, jr_nc_n, 8},
	{"LD SP, nn", 2, ld_sp_nn, 12},
	{"LDD (HL), A", 0, ldd_hlp_a, 8},
	{"INC SP", 0, inc_sp, 8},
	{"INC (HL)", 0, inc_hl, 12},
	{"DEC (HL)", 0, dec_hl, 12},
	{"LD (HL), n", 1, ld_hlp_n, 12},
	{"SCF", 0, scf, 4},
	{"JR C, n", 1, jr_c, 8},
	{"ADD HL, SP", 0, add_hl_sp, 8},
	{"LDD A, (HL)", 0, ldd_a_hl, 8},
	{"DEC SP", 0, dec_sp, 8},
	{"INC A", 0, inc_a, 4},
	{"DEC A", 0, dec_a, 4},
	{"LD A, n", 1, ld_a_n, 8},
	{"CCF", 0, ccf, 4},

	// 0x40 - 0x4F
	{"LD B, B", 0, ld_b_b, 4},
	{"LD B, C", 0, ld_b_c, 4},
	{"LD B, D", 0, ld_b_d, 4},
	{"LD B, E", 0, ld_b_e, 4},
	{"LD B, H", 0, ld_b_h, 4},
	{"LD B, L", 0, ld_b_l, 4},
	{"LD B, (HL)", 0, ld_b_hlp, 8},
	{"LD_B, A", 0, ld_b_a, 4},
	{"LD C, B", 0, ld_c_b, 4},
	{"LD C, C", 0, ld_c_c, 4},
	{"LD C, D", 0, ld_c_d, 4},
	{"LD C, E", 0, ld_c_e, 4},
	{"LD C, H", 0, ld_c_h, 4},
	{"LD C, L", 0, ld_c_l, 4},
	{"LD C, (HL)", 0, ld_c_hlp, 8},
	{"LD C, A", 0, ld_c_a, 4},

	//0x50 - 0x5F
        {"LD D, B", 0, ld_d_b, 4},
        {"LD D, C", 0, ld_d_c, 4},
        {"LD D, D", 0, ld_d_d, 4},
        {"LD D, E", 0, ld_d_e, 4},
        {"LD D, H", 0, ld_d_h, 4},
        {"LD D, L", 0, ld_d_l, 4},
        {"LD D, (HL)", 0, ld_d_hlp, 8},
        {"LD_D, A", 0, ld_d_a, 4},
        {"LD E, B", 0, ld_e_b, 4},
        {"LD E, C", 0, ld_e_c, 4},
        {"LD E, D", 0, ld_e_d, 4},
        {"LD E, E", 0, ld_e_e, 4},
        {"LD E, H", 0, ld_e_h, 4},
        {"LD E, L", 0, ld_e_l, 4},
        {"LD E, (HL)", 0, ld_e_hlp, 8},
        {"LD E, A", 0, ld_e_a, 4},

	//0x60 - 0x6F
        {"LD H, B", 0, ld_h_b, 4},
        {"LD H, C", 0, ld_h_c, 4},
        {"LD H, D", 0, ld_h_d, 4},
        {"LD H, E", 0, ld_h_e, 4},
        {"LD H, H", 0, ld_h_h, 4},
        {"LD H, L", 0, ld_h_l, 4},
        {"LD H, (HL)", 0, ld_h_hlp, 8},
        {"LD_H, A", 0, ld_h_a, 4},
        {"LD L, B", 0, ld_l_b, 4},
        {"LD L, C", 0, ld_l_c, 4},
        {"LD L, D", 0, ld_l_d, 4},
        {"LD L, E", 0, ld_l_e, 4},
        {"LD L, H", 0, ld_l_h, 4},
        {"LD L, L", 0, ld_l_l, 4},
        {"LD L, (HL)", 0, ld_l_hlp, 8},
        {"LD L, A", 0, ld_l_a, 4},

	//0x70-0x7F
        {"LD (HL), B", 0, ld_hlp_b, 8},
        {"LD (HL), C", 0, ld_hlp_c, 8},
        {"LD (HL), D", 0, ld_hlp_d, 8},
        {"LD (HL), E", 0, ld_hlp_e, 8},
        {"LD (HL), H", 0, ld_hlp_h, 8},
        {"LD (HL), L", 0, ld_hlp_l, 8},
        {"HALT", 0, halt, 4},
        {"LD_(HL), A", 0, ld_hlp_a, 8},
        {"LD A, B", 0, ld_a_b, 4},
        {"LD A, C", 0, ld_a_c, 4},
        {"LD A, D", 0, ld_a_d, 4},
        {"LD A, E", 0, ld_a_e, 4},
        {"LD A, H", 0, ld_a_h, 4},
        {"LD A, L", 0, ld_a_l, 4},
        {"LD A, (HL)", 0, ld_a_hlp, 8},
        {"LD A, A", 0, ld_a_a, 4},

	//0x80 - 0x8F
	{"ADD A, B", 0, add_a_b, 4},
	{"ADD A, C", 0, add_a_c, 4},
	{"ADD A, D", 0, add_a_d, 4},
	{"ADD A, E", 0, add_a_e, 4},
	{"ADD A, H", 0, add_a_h, 4},
	{"ADD A, L", 0, add_a_l, 4},
	{"ADD A, (HL)", 0, add_a_hlp, 8},
	{"ADD A, A", 0, add_a_a, 4},
	{"ADC A, B", 0, adc_a_b, 4},
	{"ADC A, C", 0, adc_a_c, 4},
	{"ADC A, D", 0, adc_a_d, 4},
	{"ADC A, E", 0, adc_a_e, 4},
	{"ADC A, H", 0, adc_a_h, 4},
	{"ADC A, L", 0, adc_a_l, 4},
	{"ADC A, (HL)", 0, adc_a_hlp, 8},
	{"ADC A, A", 0, adc_a_a, 4},

	//0x90 - 0x9F
	{"SUB A, B", 0, sub_a_b, 4},
	{"SUB A, C", 0, sub_a_c, 4},
	{"SUB A, D", 0, sub_a_d, 4},
	{"SUB A, E", 0, sub_a_e, 4},
	{"SUB A, H", 0, sub_a_h, 4},
	{"SUB A, L", 0, sub_a_l, 4},
	{"SUB A, (HL)", 0, sub_a_hlp, 8},
	{"SUB A, A", 0, sub_a_a, 4},
	{"SBC A, B", 0, sbc_a_b, 4},
	{"SBC A, C", 0, sbc_a_c, 4},
	{"SBC A, D", 0, sbc_a_d, 4},
	{"SBC A, E", 0, sbc_a_e, 4},
	{"SBC A, H", 0, sbc_a_h, 4},
	{"SBC A, L", 0, sbc_a_l, 4},
	{"SBC A, (HL)", 0, sbc_a_hlp, 8},
	{"SBC A, A", 0, sbc_a_a, 4},
	
	//0xA0 - 0xAF
	{"AND B", 0, and_b, 4},
	{"AND C", 0, and_c, 4},
	{"AND D", 0, and_d, 4},
	{"AND E", 0, and_e, 4},
	{"AND H", 0, and_h, 4},
	{"AND L", 0, and_l, 4},
	{"AND (HL)", 0, and_hlp, 8},
	{"AND A", 0, and_a, 4},
	{"XOR B", 0, xor_b, 4},
	{"XOR C", 0, xor_c, 4},
	{"XOR D", 0, xor_d, 4},
	{"XOR E", 0, xor_e, 4},
	{"XOR H", 0, xor_h, 4},
	{"XOR L", 0, xor_l, 4},
	{"XOR (HL)", 0, xor_hlp, 8},
	{"XOR A", 0, xor_a, 4},

	//0xB0 - 0xBF
	{"OR B", 0, or_b, 4},
	{"OR C", 0, or_c, 4},
	{"OR D", 0, or_d, 4},
	{"OR E", 0, or_e, 4},
	{"OR H", 0, or_h, 4},
	{"OR L", 0, or_l, 4},
	{"OR (HL)", 0, or_hlp, 8},
	{"OR A", 0, or_a, 4},
	{"CP B", 0, cp_b, 4},
	{"CP C", 0, cp_c, 4},
	{"CP D", 0, cp_d, 4},
	{"CP E", 0, cp_e, 4},
	{"CP H", 0, cp_h, 4},
	{"CP L", 0, cp_l, 4},
	{"CP (HL)", 0, cp_hlp, 8},
	{"CP A", 0, cp_a, 4},

	//0xC0 - 0xCF
	{"RET NZ", 0, ret_nz, 8},
	{"POP BC", 0, pop_bc, 12},
	{"JP NZ, nn", 2, jp_nz, 12},
	{"JP nn", 2, jp_nn, 12},
	{"CALL NZ, nn", 2, call_nz_nn, 12},
	{"PUSH BC", 0, push_bc, 16},
	{"ADD A, n", 1, add_a_n, 8},
	{"RST 0", 0, rst_0, 32},
	{"RET Z", 0, ret_z, 8},
	{"RET", 0, ret, 8},
	{"JP Z, nn", 2, jp_z_nn, 12},
	{"Ext ops", 0, ext_ops, 0}, //goes into other table here - extended opcodes
	{"CALL Z, nn", 2, call_z_nn, 12},
	{"CALL nn", 2, call_nn, 12},
	{"ADC A, n", 1, adc_a_n, 8},
	{"RST 8", 0, rst_8, 32},

	//0xD0 - 0xDF
        {"RET NC", 0, ret_nc, 8},
        {"POP DE", 0, pop_de, 12},
        {"JP NC, nn", 2, jp_nc, 12},
        {"UNDEFINED", 0, undefined, 0}, 
        {"CALL NC, nn", 2, call_nc_nn, 0},
        {"PUSH DE", 0, push_de, 16},
        {"SUB A, n", 1, sub_a_n, 8},
        {"RST 10", 0, rst_10, 32},
        {"RET C", 0, ret_c, 8},
        {"RETI", 0, reti, 8}, //return from interrupt
        {"JP C, nn", 2, jp_c_nn, 12},
        {"UNDEFINED", 0, undefined, 0},
        {"CALL C, nn", 2, call_c_nn, 12},
        {"UNDEFINED", 2, undefined, 0},
        {"SBC A, n", 1, sbc_a_n, 8},
        {"RST 18", 0, rst_18, 32},

	//0xE0 - 0xEF
 	{"LDH (n), A", 1, ldh_n_a, 12},
	{"POP HL", 0, pop_hl, 12},
	{"LDH (C), A", 0, ldh_c_a, 8},
	{"UNDEFINED", 0, undefined, 0},
	{"UNDEFINED", 0, undefined, 0},
	{"PUSH HL", 0, push_hl, 16},
	{"AND n", 1, and_n, 8},
	{"RST 20", 0, rest_20, 32},
	{"ADD SP, d", 0, add_sp_d, 16},
	{"JP (HL)", 0, jp_hlp, 4},
	{"LD (nn), A", 2, ld_nnp_a, 16},
	{"UNDEFINED", 0, undefined, 0},
	{"UNDEFINED", 0, undefined, 0},
	{"UNDEFINED", 0, undefined, 0},
	{"XOR n", 1, xor_n, 8},
	{"RST 28", 0, rst_28, 32},

	//0xF0 - 0xFF
        {"LDH A, (n)", 1, ldh_a_n, 12},
        {"POP AF", 0, pop_af, 12},
        {"UNDEFINED", 0, undefined, 0},
        {"DI", 0, di, 4}, //disable pending interrupt
        {"UNDEFINED", 0, undefined, 0},
        {"PUSH AF", 0, push_af, 16},
        {"OR n", 1, or_n, 8},
        {"RST 30", 0, rest_30, 32},
        {"LDHL SP, d", 0, ldhl_sp_d, 12},
        {"LD SP, HL", 0, ld_sp_hl, 8},
        {"LD A, (nn)", 2, ld_a_nnp, 16},
        {"EI", 0, ei, 4}, //enable pending interrupt
        {"UNDEFINED", 0, undefined, 0},
        {"UNDEFINED", 0, undefined, 0},
        {"CP n", 1, cp_n, 8},
        {"RST 38", 0, rst_38, 32}
	

};

uint8_t executeNextOpcode(struct gameboy * gameboy)
{
	//need to put game into main memory, sort out memory banks etc
	uint8_t opcode = readByte(gameboy, gameboy->cpu.pc++);
	const struct instruction instruction = instructions[opcode];

	printf("%x\n", gameboy->cpu.pc);

	switch(instruction.operandLength){
		case 0:
		{
			//no operands, just run it
			((void(*)(struct gameboy *))instruction.function)(gameboy);
			break;
		}
		case 1:
		{
			//1 operand, get byte
			uint8_t byte = readByte(gameboy, gameboy->cpu.pc);
			((void(*)(struct gameboy *, uint8_t))instruction.function)(gameboy, byte);
			break;
		}
		case 2:
		{
			uint16_t word = readWord(gameboy, gameboy->cpu.pc);
			((void(*)(struct gameboy *, uint16_t))instruction.function)(gameboy, word);
			break;
		}
	}

	return instruction.cycles;
}

void nop(struct gameboy * gameboy)
{
	printf("NOP\n");
}

void ld_bc_nn(struct gameboy * gameboy, uint16_t nn)
{
	printf("ld_bc_nn\n");
	gameboy->cpu.bc = nn;
}

void ld_bcp_a(struct gameboy * gameboy)
{
	printf("ld_bc_a\n"); //0x02
	//read the value in BC as an address, then write the value in A to it
	writeByte(gameboy, gameboy->cpu.bc, gameboy->cpu.a);
}

void inc_bc(struct gameboy * gameboy)
{
	printf("inc_bc\n");
	gameboy->cpu.bc++;
}

void inc_b(struct gameboy * gameboy)
{
	printf("inc_b\n");
}

void dec_b(struct gameboy * gameboy)
{
	printf("dec_b\n");
}

void ld_b_n(struct gameboy * gameboy, uint8_t n)
{
	printf("ld_b_n\n");
	gameboy->cpu.b = n;
}

void rlc_a(struct gameboy * gameboy)
{
	printf("rlc_a\n");
}

void ld_nn_sp(struct gameboy * gameboy)
{
	printf("ld_nn_sp\n");
}

void add_hl_bc(struct gameboy * gameboy)
{
	printf("add_hl_bc\n");
}

void ld_a_bcp(struct gameboy * gameboy)
{
	printf("ld_a_bcp\n");
}

void dec_bc(struct gameboy * gameboy)
{
	printf("dec_bc\n");
}

void inc_c(struct gameboy * gameboy)
{
	printf("inc_c\n");
}

void dec_c(struct gameboy * gameboy)
{
	printf("dec_c\n");
}

void ld_c_n(struct gameboy * gameboy, uint8_t n)
{
	printf("ld_c_n\n");
	gameboy->cpu.c = n;
}

void rrc_a(struct gameboy * gameboy)
{
	printf("rrc_a\n");
}

void stop(struct gameboy * gameboy) //0x10
{
	printf("stop\n");
}

void ld_de_nn(struct gameboy * gameboy, uint16_t nn) //0x11
{
	printf("ld_de_nn\n");
	gameboy->cpu.de = nn;
}

void ld_dep_a(struct gameboy * gameboy) //0x12
{
	writeByte(gameboy, gameboy->cpu.de, gameboy->cpu.a);
	printf("ld_dep_a\n");
}

void inc_de(struct gameboy * gameboy) //0x13
{
	printf("inc_de\n");
	gameboy->cpu.de++;
}

void inc_d(struct gameboy * gameboy) //0x14
{
	printf("inc_d\n");
}

void dec_d(struct gameboy * gameboy) //0x15
{
	printf("dec_d\n");
}

void ld_d_n(struct gameboy * gameboy, uint8_t n) //0x16
{
	printf("ld_d_n\n");
	gameboy->cpu.d = n;
}

void rl_a(struct gameboy * gameboy) //0x17
{
	printf("rl_a\n");
}

void jr_n(struct gameboy * gameboy) //0x18
{
	printf("jr_n\n");
}

void add_hl_de(struct gameboy * gameboy) //0x19
{
	printf("add_hl_de\n");
}

void ld_a_dep(struct gameboy * gameboy) //0x1A
{
	printf("ld_a_dep\n");
}

void dec_de(struct gameboy * gameboy) //0x1B
{
	printf("dec_de\n");
}

void inc_e(struct gameboy * gameboy) //0x1C
{
	printf("inc_e\n");
}

void dec_e(struct gameboy * gameboy) //0x1D
{
	printf("dec_e\n");
}

void ld_e_n(struct gameboy * gameboy, uint8_t n) //0x1E
{
	printf("ld_e_n\n");
	gameboy->cpu.e = n;
}

void rr_a(struct gameboy * gameboy) //0x1F
{
	printf("rr_a\n");
}

void jr_nz_n(struct gameboy * gameboy)
{
	printf("jr_nz_n\n");
}

void ld_hl_nn(struct gameboy * gameboy, uint16_t nn)
{
	printf("ld_hl_nn\n");
	gameboy->cpu.hl = nn;
}

void ldi_hlp_a(struct gameboy * gameboy)
{
	printf("ldi_hlp_a\n");
	writeByte(gameboy, gameboy->cpu.hl, gameboy->cpu.a);
}

void inc_hl(struct gameboy * gameboy)
{
	printf("inc_hl\n");
	gameboy->cpu.hl++;
}

void inc_h(struct gameboy * gameboy)
{
	printf("inc_h\n");
}

void dec_h(struct gameboy * gameboy)
{
	printf("dec_h\n");
}

void ld_h_n(struct gameboy * gameboy, uint8_t n)
{
	printf("ld_h_n\n");
	gameboy->cpu.h = n;
}

void daa(struct gameboy * gameboy)
{
	printf("daa\n");
}

void jr_z_n(struct gameboy * gameboy)
{
	printf("jr_z_n\n");
}

void add_hl_hl(struct gameboy * gameboy)
{
	printf("add_hl_hl\n");
}

void ldi_a_hlp(struct gameboy * gameboy)
{
	printf("ldi_a_hlp\n");
}

void dec_hl(struct gameboy * gameboy)
{
	printf("dec_hl\n");
}

void ld_l_n(struct gameboy * gameboy, uint8_t n)
{
	printf("ld_l_n\n");
	gameboy->cpu.l = n;
}

void cpl(struct gameboy * gameboy)
{
	printf("cpl\n");
}

void jr_nc_n(struct gameboy * gameboy)
{
	printf("jr_nc_n\n");
}

void ld_sp_nn(struct gameboy * gameboy, uint16_t nn)
{
	printf("ld_sp_nn\n");
	gameboy->cpu.sp = nn;
}

void ldd_hlp_a(struct gameboy * gameboy)
{
	printf("ldd_hlp_a\n");
	
}

void inc_sp(struct gameboy * gameboy)
{
	printf("inc_sp\n");
	gameboy->cpu.sp++;
}

void inc_hlp(struct gameboy * gameboy)
{
	printf("inc_hlp\n");
}

void dec_hlp(struct gameboy * gameboy)
{
	printf("dec_hlp\n");
}

void ld_hlp_n(struct gameboy * gameboy, uint8_t data)
{
	printf("ld_hlp_n\n");
	writeByte(gameboy, gameboy->cpu.hl, data);
}

void scf(struct gameboy * gameboy)
{
	printf("scf\n");
}

void jr_c(struct gameboy * gameboy)
{
	printf("jr_c\n");
}

void add_hl_sp(struct gameboy * gameboy)
{
	printf("add_hl_sp\n");
}

void ldd_a_hl(struct gameboy * gameboy)
{
	printf("ldd_a_hl\n");
}

void dec_sp(struct gameboy * gameboy)
{
	printf("dec_sp\n");
}

void inc_a(struct gameboy * gameboy)
{
	printf("inc_a\n");
}

void dec_a(struct gameboy * gameboy)
{
	printf("dec_a\n");
}

void ld_a_n(struct gameboy * gameboy, uint8_t n)
{
	printf("ld_a_n\n");
	gameboy->cpu.a = n;
}

void ccf(struct gameboy * gameboy)
{
	printf("ccf\n");
}

        // 0x40 - 0x4F
void ld_b_b(struct gameboy * gameboy)
{
	printf("ld_b_b\n");
	//do nothing
}

void ld_b_c(struct gameboy * gameboy)
{
	printf("ld_b_c\n");
	gameboy->cpu.b = gameboy->cpu.c;
}

void ld_b_d(struct gameboy * gameboy)
{
	printf("ld_b_d\n");
	gameboy->cpu.b = gameboy->cpu.d;
}

void ld_b_e(struct gameboy * gameboy)
{
	printf("ld_b_e\n");
	gameboy->cpu.b = gameboy->cpu.e;
}

void ld_b_h(struct gameboy * gameboy)
{
	printf("ld_b_h\n");
	gameboy->cpu.b = gameboy->cpu.h;
}

void ld_b_l(struct gameboy * gameboy)
{
	printf("ld_b_l\n");
	gameboy->cpu.b = gameboy->cpu.l;
}

void ld_b_hlp(struct gameboy * gameboy)
{
	printf("ld_b_hlp\n");
	gameboy->cpu.b = readByte(gameboy, gameboy->cpu.hl);
}

void ld_b_a(struct gameboy * gameboy)
{
	printf("ld_b_a\n");
	gameboy->cpu.b = gameboy->cpu.a;
}

void ld_c_b(struct gameboy * gameboy)
{
	printf("ld_c_b\n");
	gameboy->cpu.c = gameboy->cpu.b;
}

void ld_c_c(struct gameboy * gameboy)
{
	printf("ld_c_c\n");
	//do nothing
}

void ld_c_d(struct gameboy * gameboy)
{
	printf("ld_c_d\n");
	gameboy->cpu.c = gameboy->cpu.d;
}

void ld_c_e(struct gameboy * gameboy)
{
	printf("ld_c_e\n");
	gameboy->cpu.c = gameboy->cpu.e;
}

void ld_c_h(struct gameboy * gameboy)
{
	printf("ld_c_h\n");
	gameboy->cpu.c = gameboy->cpu.h;
}

void ld_c_l(struct gameboy * gameboy)
{
	printf("ld_c_l\n");
	gameboy->cpu.c = gameboy->cpu.l;
}

void ld_c_hlp(struct gameboy * gameboy)
{
	printf("ld_c_hlp\n");
	gameboy->cpu.c = readByte(gameboy, gameboy->cpu.hl);
}

void ld_c_a(struct gameboy * gameboy)
{
	printf("ld_c_a\n");
	gameboy->cpu.c = gameboy->cpu.a;
}



//0x50 - 0x5F

void ld_d_b(struct gameboy * gameboy)
{
	printf("ld_d_b\n");
	gameboy->cpu.d = gameboy->cpu.b;
}

void ld_d_c(struct gameboy * gameboy)
{
	printf("ld_d_c\n");
	gameboy->cpu.d = gameboy->cpu.c;
}

void ld_d_d(struct gameboy * gameboy)
{
	printf("ld_d_d\n");
	//do nothing
}

void ld_d_e(struct gameboy * gameboy)
{
	printf("ld_d_e\n");
	gameboy->cpu.d = gameboy->cpu.e;
}

void ld_d_h(struct gameboy * gameboy)
{
	printf("ld_d_h\n");
	gameboy->cpu.d = gameboy->cpu.h;
}

void ld_d_l(struct gameboy * gameboy)
{
	printf("ld_d_l\n");
	gameboy->cpu.d = gameboy->cpu.l;
}

void ld_d_hlp(struct gameboy * gameboy)
{
	printf("ld_d_hlp\n");
	gameboy->cpu.d = readByte(gameboy, gameboy->cpu.hl);
}

void ld_d_a(struct gameboy * gameboy)
{
	printf("ld_d_a\n");
	gameboy->cpu.d = gameboy->cpu.a;
}

void ld_e_b(struct gameboy * gameboy)
{
	printf("ld_e_b\n");
	gameboy->cpu.e = gameboy->cpu.b;
}

void ld_e_c(struct gameboy * gameboy)
{
	printf("ld_e_c\n");
	gameboy->cpu.e = gameboy->cpu.c;
}

void ld_e_d(struct gameboy * gameboy)
{
	printf("ld_e_d\n");
	gameboy->cpu.e = gameboy->cpu.d;
}

void ld_e_e(struct gameboy * gameboy)
{
	printf("ld_e_e\n");
	//do nothing
}

void ld_e_h(struct gameboy * gameboy)
{
	printf("ld_e_h\n");
	gameboy->cpu.e = gameboy->cpu.h;
}

void ld_e_l(struct gameboy * gameboy)
{
	printf("ld_e_l\n");
	gameboy->cpu.e = gameboy->cpu.l;
}

void ld_e_hlp(struct gameboy * gameboy)
{
	printf("ld_e_hlp\n");
	gameboy->cpu.e = readByte(gameboy, gameboy->cpu.hl);
}

void ld_e_a(struct gameboy * gameboy)
{
	printf("ld_e_a\n");
	gameboy->cpu.e = gameboy->cpu.a;
}

//0x60 - 0x6F

void ld_h_b(struct gameboy * gameboy)
{
	printf("ld_h_b\n");
	gameboy->cpu.h = gameboy->cpu.b;
}

void ld_h_c(struct gameboy * gameboy)
{
	printf("ld_h_c\n");
	gameboy->cpu.h = gameboy->cpu.c;
}

void ld_h_d(struct gameboy * gameboy)
{
	printf("ld_h_d\n");
	gameboy->cpu.h = gameboy->cpu.d;
}

void ld_h_e(struct gameboy * gameboy)
{
	printf("ld_h_e\n");
	gameboy->cpu.h = gameboy->cpu.e;
}

void ld_h_h(struct gameboy * gameboy)
{
	printf("ld_h_h\n");
	//do nothing
}

void ld_h_l(struct gameboy * gameboy)
{
	printf("ld_h_l\n");
	gameboy->cpu.h = gameboy->cpu.l;
}

void ld_h_hlp(struct gameboy * gameboy)
{
	printf("ld_h_hlp\n");
	gameboy->cpu.h = readByte(gameboy, gameboy->cpu.hl);
}

void ld_h_a(struct gameboy * gameboy)
{
	printf("ld_h_a\n");
	gameboy->cpu.h = gameboy->cpu.a;
}

void ld_l_b(struct gameboy * gameboy)
{
	printf("ld_l_b\n");
	gameboy->cpu.l = gameboy->cpu.b;
}

void ld_l_c(struct gameboy * gameboy)
{
	printf("ld_l_c\n");
	gameboy->cpu.l = gameboy->cpu.c;
}

void ld_l_d(struct gameboy * gameboy)
{
	printf("ld_l_d\n");
	gameboy->cpu.l = gameboy->cpu.d;
}

void ld_l_e(struct gameboy * gameboy)
{
	printf("ld_l_e\n");
	gameboy->cpu.l = gameboy->cpu.e;
}

void ld_l_h(struct gameboy * gameboy)
{
	printf("ld_l_h\n");
	gameboy->cpu.l = gameboy->cpu.h;
}

void ld_l_l(struct gameboy * gameboy)
{
	printf("ld_l_l\n");
	//do nothing
}

void ld_l_hlp(struct gameboy * gameboy)
{
	printf("ld_l_hlp\n");
	gameboy->cpu.l = readByte(gameboy, gameboy->cpu.hl);
}

void ld_l_a(struct gameboy * gameboy)
{
	printf("ld_l_a\n");
	gameboy->cpu.l = gameboy->cpu.a;
}

//0x70-0x7F

void ld_hlp_b(struct gameboy * gameboy)
{
	printf("ld_hlp_b\n");
	writeByte(gameboy, gameboy->cpu.hl, gameboy->cpu.b);
}

void ld_hlp_c(struct gameboy * gameboy)
{
	printf("ld_hlp_c\n");
	writeByte(gameboy, gameboy->cpu.hl, gameboy->cpu.c);
}

void ld_hlp_d(struct gameboy * gameboy)
{
	printf("ld_hlp_d\n");
	writeByte(gameboy, gameboy->cpu.hl, gameboy->cpu.d);
}

void ld_hlp_e(struct gameboy * gameboy)
{
	printf("ld_hlp_e\n");
	writeByte(gameboy, gameboy->cpu.hl, gameboy->cpu.e);
}

void ld_hlp_h(struct gameboy * gameboy)
{
	printf("ld_hlp_h\n");
	writeByte(gameboy, gameboy->cpu.hl, gameboy->cpu.h);
}

void ld_hlp_l(struct gameboy * gameboy)
{
	printf("ld_hlp_l\n");
	writeByte(gameboy, gameboy->cpu.hl, gameboy->cpu.l);
}

void halt(struct gameboy * gameboy)
{
	printf("halt\n");
}

void ld_hlp_a(struct gameboy * gameboy)
{
	printf("ld_hlp_a\n");
	writeByte(gameboy, gameboy->cpu.hl, gameboy->cpu.a);
}

void ld_a_b(struct gameboy * gameboy)
{
	printf("ld_a_b\n");
	gameboy->cpu.a = gameboy->cpu.b;
}

void ld_a_c(struct gameboy * gameboy)
{
	printf("ld_a_c\n");
	gameboy->cpu.a = gameboy->cpu.c;
}

void ld_a_d(struct gameboy * gameboy)
{
	printf("ld_a_d\n");
	gameboy->cpu.a = gameboy->cpu.d;
}

void ld_a_e(struct gameboy * gameboy)
{
	printf("ld_a_e\n");
	gameboy->cpu.a = gameboy->cpu.e;
}

void ld_a_h(struct gameboy * gameboy)
{
	printf("ld_a_h\n");
	gameboy->cpu.a = gameboy->cpu.h;
}

void ld_a_l(struct gameboy * gameboy)
{
	printf("ld_a_l\n");
	gameboy->cpu.a = gameboy->cpu.l;
}

void ld_a_hlp(struct gameboy * gameboy)
{
	printf("ld_a_hlp\n");
	gameboy->cpu.a = readByte(gameboy, gameboy->cpu.hl);
}

void ld_a_a(struct gameboy * gameboy)
{
	printf("ld_a_a\n");
	gameboy->cpu.a = gameboy->cpu.a;
}

//0x80 - 0x8F

void add_a_b(struct gameboy * gameboy)
{
	printf("add_a_b\n");
}

void add_a_c(struct gameboy * gameboy)
{
	printf("add_a_c\n");
}

void add_a_d(struct gameboy * gameboy)
{
	printf("add_a_d\n");
}

void add_a_e(struct gameboy * gameboy)
{
	printf("add_a_e\n");
}

void add_a_h(struct gameboy * gameboy)
{
	printf("add_a_h\n");
}

void add_a_l(struct gameboy * gameboy)
{
	printf("add_a_l\n");
}

void add_a_hlp(struct gameboy * gameboy)
{
	printf("add_a_hlp\n");
}

void add_a_a(struct gameboy * gameboy)
{
	printf("add_a_a\n");
}

void adc_a_b(struct gameboy * gameboy)
{
	printf("adc_a_b\n");
}

void adc_a_c(struct gameboy * gameboy)
{
	printf("adc_a_c\n");
}

void adc_a_d(struct gameboy * gameboy)
{
	printf("adc_a_d\n");
}

void adc_a_e(struct gameboy * gameboy)
{
	printf("adc_a_e\n");
}

void adc_a_h(struct gameboy * gameboy)
{
	printf("adc_a_h\n");
}

void adc_a_l(struct gameboy * gameboy)
{
	printf("adc_a_l\n");
}

void adc_a_hlp(struct gameboy * gameboy)
{
	printf("adc_a_hlp\n");
}

void adc_a_a(struct gameboy * gameboy)
{
	printf("adc_a_a\n");
}



 //0x90 - 0x9F

void sub_a_b(struct gameboy * gameboy)
{
	printf("sub_a_b\n");
}

void sub_a_c(struct gameboy * gameboy)
{
	printf("sub_a_c\n");
}

void sub_a_d(struct gameboy * gameboy)
{
	printf("sub_a_d\n");
}

void sub_a_e(struct gameboy * gameboy)
{
	printf("sub_a_e\n");
}

void sub_a_h(struct gameboy * gameboy)
{
	printf("sub_a_h\n");
}

void sub_a_l(struct gameboy * gameboy)
{
	printf("sub_a_l\n");
}

void sub_a_hlp(struct gameboy * gameboy)
{
	printf("sub_a_hlp\n");
}

void sub_a_a(struct gameboy * gameboy)
{
	printf("sub_a_a\n");
}

void sbc_a_b(struct gameboy * gameboy)
{
	printf("sbc_a_b\n");
}

void sbc_a_c(struct gameboy * gameboy)
{
	printf("sbc_a_c\n");
}

void sbc_a_d(struct gameboy * gameboy)
{
	printf("sbc_a_d\n");
}

void sbc_a_e(struct gameboy * gameboy)
{
	printf("sbc_a_e\n");
}

void sbc_a_h(struct gameboy * gameboy)
{
	printf("sbc_a_h\n");
}

void sbc_a_l(struct gameboy * gameboy)
{
	printf("sbc_a_l\n");
}

void sbc_a_hlp(struct gameboy * gameboy)
{
	printf("sbc_a_hlp\n");
}

void sbc_a_a(struct gameboy * gameboy)
{
	printf("sbc_a_a\n");
}



        //0xA0 - 0xAF

void and_b(struct gameboy * gameboy)
{
	printf("and_b\n");
}

void and_c(struct gameboy * gameboy)
{
	printf("and_c\n");
}

void and_d(struct gameboy * gameboy)
{
	printf("and_d\n");
}

void and_e(struct gameboy * gameboy)
{
	printf("and_e\n");
}

void and_h(struct gameboy * gameboy)
{
	printf("and_h\n");
}

void and_l(struct gameboy * gameboy)
{
	printf("and_l\n");
}

void and_hlp(struct gameboy * gameboy)
{
	printf("and_hlp\n");
}

void and_a(struct gameboy * gameboy)
{
	printf("and_a\n");
}

void xor_b(struct gameboy * gameboy)
{
	printf("xor_b\n");
}

void xor_c(struct gameboy * gameboy)
{
	printf("xor_c\n");
}

void xor_d(struct gameboy * gameboy)
{
	printf("xor_d\n");
}

void xor_e(struct gameboy * gameboy)
{
	printf("xor_e\n");
}

void xor_h(struct gameboy * gameboy)
{
	printf("xor_h\n");
}

void xor_l(struct gameboy * gameboy)
{
	printf("xor_l\n");
}

void xor_hlp(struct gameboy * gameboy)
{
	printf("xor_hlp\n");
}

void xor_a(struct gameboy * gameboy)
{
	printf("xor_a\n");
}



//0xB0 - 0xBF

void or_b(struct gameboy * gameboy)
{
	printf("or_b\n");
}

void or_c(struct gameboy * gameboy)
{
	printf("or_c\n");
}

void or_d(struct gameboy * gameboy)
{
	printf("or_d\n");
}

void or_e(struct gameboy * gameboy)
{
	printf("or_e\n");
}

void or_h(struct gameboy * gameboy)
{
	printf("or_h\n");
}

void or_l(struct gameboy * gameboy)
{
	printf("or_l\n");
}

void or_hlp(struct gameboy * gameboy)
{
	printf("or_hlp\n");
}

void or_a(struct gameboy * gameboy)
{
	printf("or_a\n");
}

void cp_b(struct gameboy * gameboy)
{
	printf("cp_b\n");
}

void cp_c(struct gameboy * gameboy)
{
	printf("cp_c\n");
}

void cp_d(struct gameboy * gameboy)
{
	printf("cp_d\n");
}

void cp_e(struct gameboy * gameboy)
{
	printf("cp_e\n");
}

void cp_h(struct gameboy * gameboy)
{
	printf("cp_h\n");
}

void cp_l(struct gameboy * gameboy)
{
	printf("cp_l\n");
}

void cp_hlp(struct gameboy * gameboy)
{
	printf("cp_hlp\n");
}

void cp_a(struct gameboy * gameboy)
{
	printf("cp_a\n");
}



        //0xC0 - 0xCF

void ret_nz(struct gameboy * gameboy)
{
	printf("ret_nz\n");
}

void pop_bc(struct gameboy * gameboy)
{
	printf("pop_bc\n");
	gameboy->cpu.bc = popWordFromStack(gameboy);
}

void jp_nz(struct gameboy * gameboy)
{
	printf("jp_nz\n");
}

void jp_nn(struct gameboy * gameboy)
{
	printf("jp_nn\n");
}

void call_nz_nn(struct gameboy * gameboy)
{
	printf("call_nz_nn\n");
}

void push_bc(struct gameboy * gameboy)
{
	printf("push_bc\n");
}

void add_a_n(struct gameboy * gameboy)
{
	printf("add_a_n\n");
}

void rst_0(struct gameboy * gameboy)
{
	printf("rst_0\n");
}

void ret_z(struct gameboy * gameboy)
{
	printf("ret_z\n");
}

void ret(struct gameboy * gameboy)
{
	printf("ret\n");
}

void jp_z_nn(struct gameboy * gameboy)
{
	printf("jp_z_nn\n");
}

void ext_ops(struct gameboy * gameboy)
{
	printf("ext_ops\n");
}

void call_z_nn(struct gameboy * gameboy)
{
	printf("call_z_nn\n");
}

void call_nn(struct gameboy * gameboy)
{
	printf("call_nn\n");
}

void adc_a_n(struct gameboy * gameboy)
{
	printf("adc_a_n\n");
}

void rst_8(struct gameboy * gameboy)
{
	printf("rst_8\n");
}



//0xD0 - 0xDF

void ret_nc(struct gameboy * gameboy)
{
	printf("ret_nc\n");
}

void pop_de(struct gameboy * gameboy)
{
	printf("pop_de\n");
	//pop top value from stack into register DE
	gameboy->cpu.de = popWordFromStack(gameboy);
}

void jp_nc(struct gameboy * gameboy)
{
	printf("jp_nc\n");
}

void call_nc_nn(struct gameboy * gameboy)
{
	printf("call_nc_nn\n");
}

void push_de(struct gameboy * gameboy)
{
	printf("push_de\n");
	pushWordToStack(gameboy, gameboy->cpu.de);
}

void sub_a_n(struct gameboy * gameboy)
{
	printf("sub_a_n\n");
}

void rst_10(struct gameboy * gameboy)
{
	printf("rst_10\n");
}

void ret_c(struct gameboy * gameboy)
{
	printf("ret_c\n");
}


void reti(struct gameboy * gameboy)
{
	printf("reti\n");
}

void jp_c_nn(struct gameboy * gameboy)
{
	printf("jp_c_nn\n");
}

void call_c_nn(struct gameboy * gameboy)
{
	printf("call_c_nn\n");
}

void sbc_a_n(struct gameboy * gameboy)
{
	printf("sbc_a_n\n");
}

void rst_18(struct gameboy * gameboy)
{
	printf("rst_18\n");
}



        //0xE0 - 0xEF

void ldh_n_a(struct gameboy * gameboy, uint8_t n)
{
	printf("ldh_n_a\n");
	//put a into memory address FF00+n
	writeByte(gameboy, LDH_BASE + n, gameboy->cpu.a);
}

void pop_hl(struct gameboy * gameboy)
{
	printf("pop_hl\n");
}

void ldh_c_a(struct gameboy * gameboy)
{
	printf("ldh_c_a\n");
	//put A into address FF00 + reg C
	writeByte(gameboy, LDH_BASE + gameboy->cpu.c, gameboy->cpu.a);
}

void push_hl(struct gameboy * gameboy)
{
	printf("push_hl\n");
}

void and_n(struct gameboy * gameboy)
{
	printf("and_n\n");
}

void rest_20(struct gameboy * gameboy)
{
	printf("rest_20\n");
}

void add_sp_d(struct gameboy * gameboy)
{
	printf("add_sp_d\n");
}

void jp_hlp(struct gameboy * gameboy)
{
	printf("jp_hlp\n");
}

void ld_nnp_a(struct gameboy * gameboy, uint16_t nnp)
{
	printf("ld_nnp_a\n");
	//load value in a to address nnp
	writeByte(gameboy, nnp, gameboy->cpu.a);	
}

void xor_n(struct gameboy * gameboy)
{
	printf("xor_n\n");
}

void rst_28(struct gameboy * gameboy)
{
	printf("rst_28\n");
}

  //0xF0 - 0xFF

void ldh_a_n(struct gameboy * gameboy, uint8_t n)
{
	printf("ldh_a_n\n");
	//put memory address FF00 + n into A
	gameboy->cpu.a = readByte(gameboy, LDH_BASE + n);
	
}

void pop_af(struct gameboy * gameboy)
{
	printf("pop_af\n");
}

void di(struct gameboy * gameboy)
{
	printf("di\n");
	setInterruptMasterFlag(gameboy, false);
}

void push_af(struct gameboy * gameboy)
{
	printf("push_af\n");
}

void or_n(struct gameboy * gameboy)
{
	printf("or_n\n");
}

void rest_30(struct gameboy * gameboy)
{
	printf("rest_30\n");
}

void ldhl_sp_d(struct gameboy * gameboy)
{
	printf("ldhl_sp_d\n");
	
}

void ld_sp_hl(struct gameboy * gameboy)
{
	printf("ld_sp_hl\n");
	gameboy->cpu.sp = gameboy->cpu.hl;
}

void ld_a_nnp(struct gameboy * gameboy, uint16_t nnp)
{
	printf("ld_a_nnp\n");
	//read data as address
	gameboy->cpu.a = readByte(gameboy, nnp);
}

void ei(struct gameboy * gameboy)
{
	printf("ei\n");
	//enable interrupt
	setInterruptMasterFlag(gameboy, true);
}

void cp_n(struct gameboy * gameboy)
{
	printf("cp_n\n");
}

void rst_38(struct gameboy * gameboy)
{
	printf("rst_38\n");
}


void undefined(struct gameboy * gameboy)
{
	printf("undefined\n");
	gameboy->cpu.pc--;
}
