#ifndef CPU_H
#define CPU_H

#include <stdint.h>

/*
Details:

8 bit - all internal workings operate on one byte at a time
CPU fetches instructions from main memory, decodes it, then executes it.
Contains a Program Counter that keeps track of where the CPU is in the program.
The PC is advanced by however many fetched bytes make up the instruction

Registers: A, B, C, D, E, H and L - 1 byte each, so values from 0 - 255

If there are 256 possible values in the first byte of an instruction, that
makes for 256 possible instructions in the basic opcode table. Each instruction
can be simulated with a function that operates on an internal model of the
registers, and produces effects on an internal model of the memory interface.

Register F is the flag register. Flags are:
	ZERO - Set if the last operation produces a 0 (0x80)
	OPERATION - Set if the last operation was a subtraction (0x40)
	HALF-CARRY - Set if the lower half of the byte result of the last 
	operation overflowed past 15. (0x20)
	CARRY - Set if the last operation produced a result over 255 for 
	additions or under 0 for subtractions (0x10)

Represent 1 byte as an unsigned char

*/

//4194304Hz
#define CLOCKSPEED 4194304

#define NO_OF_INSTRUCTIONS 256

/*
Struct that represents an instruction, complete with a pointer to a function
*/
struct instruction {
	char * instruction;
	uint8_t operandLength; //length of the operands for this opcode
	void * function;
	uint8_t cycles; //CPU cycles it takes to execute
};

/*
Split gameboy struct into CPU, memory etc structs, then pass into functions
as need be.
*/

struct cpu {
	//union reg AF;
	//union reg BC;
	//union reg DE;
	//union reg HL;

	struct {
		union {
			uint16_t af;
			struct {
				uint8_t f;
				uint8_t a;
			};
		};
	};

	struct {
		union {
			uint16_t bc;
			struct {
				uint8_t c;
				uint8_t b;
			};
		};
	};

	struct {
		union {
			uint16_t de;
			struct {
				uint8_t e;
				uint8_t d;
			};
		};
	};

	struct {
		union {
			uint16_t hl;
			struct {
				uint8_t l;
				uint8_t h;
			};
		};
	};
	
	uint16_t sp;
	uint16_t pc;

};

#define LDH_BASE 0xFF00

struct gameboy;

uint8_t executeNextOpcode(struct gameboy * gameboy);

// 0x00 - 0x0F
void nop(struct gameboy * gameboy); //0
void ld_bc_nn(struct gameboy * gameboy, uint16_t nn); //1
void ld_bcp_a(struct gameboy * gameboy); //2
void inc_bc(struct gameboy * gameboy); //3
void inc_b(struct gameboy * gameboy); //4
void dec_b(struct gameboy * gameboy); //5
void ld_b_n(struct gameboy * gameboy, uint8_t n); //6
void rlc_a(struct gameboy * gameboy); //7
void ld_nn_sp(struct gameboy * gameboy); //8
void add_hl_bc(struct gameboy * gameboy); //9
void ld_a_bcp(struct gameboy * gameboy); //A
void dec_bc(struct gameboy * gameboy); //B
void inc_c(struct gameboy * gameboy); //C
void dec_c(struct gameboy * gameboy); //D
void ld_c_n(struct gameboy * gameboy, uint8_t n); //E
void rrc_a(struct gameboy * gameboy); //F

// 0x10 - 0x1F
void stop(struct gameboy * gameboy); //0x10
void ld_de_nn(struct gameboy * gameboy, uint16_t nn); //0x11
void ld_dep_a(struct gameboy * gameboy); //0x12
void inc_de(struct gameboy * gameboy); //0x13
void inc_d(struct gameboy * gameboy); //0x14
void dec_d(struct gameboy * gameboy); //0x15
void ld_d_n(struct gameboy * gameboy, uint8_t n); //0x16
void rl_a(struct gameboy * gameboy); //0x17
void jr_n(struct gameboy * gameboy); //0x18
void add_hl_de(struct gameboy * gameboy); //0x19
void ld_a_dep(struct gameboy * gameboy); //0x1A
void dec_de(struct gameboy * gameboy); //0x1B
void inc_e(struct gameboy * gameboy); //0x1C
void dec_e(struct gameboy * gameboy); //0x1D
void ld_e_n(struct gameboy * gameboy, uint8_t n); //0x1E
void rr_a(struct gameboy * gameboy); //0x1F

//0x20 - 0x2F
void jr_nz_n(struct gameboy * gameboy);
void ld_hl_nn(struct gameboy * gameboy, uint16_t nn);
void ldi_hlp_a(struct gameboy * gameboy);
void inc_hl(struct gameboy * gameboy);
void inc_h(struct gameboy * gameboy);
void dec_h(struct gameboy * gameboy);
void ld_h_n(struct gameboy * gameboy, uint8_t n);
void daa(struct gameboy * gameboy);
void jr_z_n(struct gameboy * gameboy);
void add_hl_hl(struct gameboy * gameboy);
void ldi_a_hlp(struct gameboy * gameboy);
void dec_hl(struct gameboy * gameboy);
void ld_l_n(struct gameboy * gameboy, uint8_t n);
void cpl(struct gameboy * gameboy);

// 0x30 - 0x3F
void jr_nc_n(struct gameboy * gameboy);
void ld_sp_nn(struct gameboy * gameboy, uint16_t nn);
void ldd_hlp_a(struct gameboy * gameboy);
void inc_sp(struct gameboy * gameboy);
void inc_hl(struct gameboy * gameboy);
void dec_hl(struct gameboy * gameboy);
void ld_hlp_n(struct gameboy * gameboy, uint8_t data);
void scf(struct gameboy * gameboy);
void jr_c(struct gameboy * gameboy);
void add_hl_sp(struct gameboy * gameboy);
void ldd_a_hl(struct gameboy * gameboy);
void dec_sp(struct gameboy * gameboy);
void inc_a(struct gameboy * gameboy);
void dec_a(struct gameboy * gameboy);
void ld_a_n(struct gameboy * gameboy, uint8_t n);
void ccf(struct gameboy * gameboy);

        // 0x40 - 0x4F
void ld_b_b(struct gameboy * gameboy);
void ld_b_c(struct gameboy * gameboy);
void ld_b_d(struct gameboy * gameboy);
void ld_b_e(struct gameboy * gameboy);
void ld_b_h(struct gameboy * gameboy);
void ld_b_l(struct gameboy * gameboy);
void ld_b_hlp(struct gameboy * gameboy);
void ld_b_a(struct gameboy * gameboy);
void ld_c_b(struct gameboy * gameboy);
void ld_c_c(struct gameboy * gameboy);
void ld_c_d(struct gameboy * gameboy);
void ld_c_e(struct gameboy * gameboy);
void ld_c_h(struct gameboy * gameboy);
void ld_c_l(struct gameboy * gameboy);
void ld_c_hlp(struct gameboy * gameboy);
void ld_c_a(struct gameboy * gameboy);

//0x50 - 0x5F
void ld_d_b(struct gameboy * gameboy);
void ld_d_c(struct gameboy * gameboy);
void ld_d_d(struct gameboy * gameboy);
void ld_d_e(struct gameboy * gameboy);
void ld_d_h(struct gameboy * gameboy);
void ld_d_l(struct gameboy * gameboy);
void ld_d_hlp(struct gameboy * gameboy);
void ld_d_a(struct gameboy * gameboy);
void ld_e_b(struct gameboy * gameboy);
void ld_e_c(struct gameboy * gameboy);
void ld_e_d(struct gameboy * gameboy);
void ld_e_e(struct gameboy * gameboy);
void ld_e_h(struct gameboy * gameboy);
void ld_e_l(struct gameboy * gameboy);
void ld_e_hlp(struct gameboy * gameboy);
void ld_e_a(struct gameboy * gameboy);

        //0x60 - 0x6F
void ld_h_b(struct gameboy * gameboy);
void ld_h_c(struct gameboy * gameboy);
void ld_h_d(struct gameboy * gameboy);
void ld_h_e(struct gameboy * gameboy);
void ld_h_h(struct gameboy * gameboy);
void ld_h_l(struct gameboy * gameboy);
void ld_h_hlp(struct gameboy * gameboy);
void ld_h_a(struct gameboy * gameboy);
void ld_l_b(struct gameboy * gameboy);
void ld_l_c(struct gameboy * gameboy);
void ld_l_d(struct gameboy * gameboy);
void ld_l_e(struct gameboy * gameboy);
void ld_l_h(struct gameboy * gameboy);
void ld_l_l(struct gameboy * gameboy);
void ld_l_hlp(struct gameboy * gameboy);
void ld_l_a(struct gameboy * gameboy);

//0x70-0x7F
void ld_hlp_b(struct gameboy * gameboy);
void ld_hlp_c(struct gameboy * gameboy);
void ld_hlp_d(struct gameboy * gameboy);
void ld_hlp_e(struct gameboy * gameboy);
void ld_hlp_h(struct gameboy * gameboy);
void ld_hlp_l(struct gameboy * gameboy);
void halt(struct gameboy * gameboy);
void ld_hlp_a(struct gameboy * gameboy);
void ld_a_b(struct gameboy * gameboy);
void ld_a_c(struct gameboy * gameboy);
void ld_a_d(struct gameboy * gameboy);
void ld_a_e(struct gameboy * gameboy);
void ld_a_h(struct gameboy * gameboy);
void ld_a_l(struct gameboy * gameboy);
void ld_a_hlp(struct gameboy * gameboy);
void ld_a_a(struct gameboy * gameboy);

        //0x80 - 0x8F
void add_a_b(struct gameboy * gameboy);
void add_a_c(struct gameboy * gameboy);
void add_a_d(struct gameboy * gameboy);
void add_a_e(struct gameboy * gameboy);
void add_a_h(struct gameboy * gameboy);
void add_a_l(struct gameboy * gameboy);
void add_a_hlp(struct gameboy * gameboy);
void add_a_a(struct gameboy * gameboy);
void adc_a_b(struct gameboy * gameboy);
void adc_a_c(struct gameboy * gameboy);
void adc_a_d(struct gameboy * gameboy);
void adc_a_e(struct gameboy * gameboy);
void adc_a_h(struct gameboy * gameboy);
void adc_a_l(struct gameboy * gameboy);
void adc_a_hlp(struct gameboy * gameboy);
void adc_a_a(struct gameboy * gameboy);

 //0x90 - 0x9F
void sub_a_b(struct gameboy * gameboy);
void sub_a_c(struct gameboy * gameboy);
void sub_a_d(struct gameboy * gameboy);
void sub_a_e(struct gameboy * gameboy);
void sub_a_h(struct gameboy * gameboy);
void sub_a_l(struct gameboy * gameboy);
void sub_a_hlp(struct gameboy * gameboy);
void sub_a_a(struct gameboy * gameboy);
void sbc_a_b(struct gameboy * gameboy);
void sbc_a_c(struct gameboy * gameboy);
void sbc_a_d(struct gameboy * gameboy);
void sbc_a_e(struct gameboy * gameboy);
void sbc_a_h(struct gameboy * gameboy);
void sbc_a_l(struct gameboy * gameboy);
void sbc_a_hlp(struct gameboy * gameboy);
void sbc_a_a(struct gameboy * gameboy);

        //0xA0 - 0xAF
void and_b(struct gameboy * gameboy);
void and_c(struct gameboy * gameboy);
void and_d(struct gameboy * gameboy);
void and_e(struct gameboy * gameboy);
void and_h(struct gameboy * gameboy);
void and_l(struct gameboy * gameboy);
void and_hlp(struct gameboy * gameboy);
void and_a(struct gameboy * gameboy);
void xor_b(struct gameboy * gameboy);
void xor_c(struct gameboy * gameboy);
void xor_d(struct gameboy * gameboy);
void xor_e(struct gameboy * gameboy);
void xor_h(struct gameboy * gameboy);
void xor_l(struct gameboy * gameboy);
void xor_hlp(struct gameboy * gameboy);
void xor_a(struct gameboy * gameboy);

//0xB0 - 0xBF
void or_b(struct gameboy * gameboy);
void or_c(struct gameboy * gameboy);
void or_d(struct gameboy * gameboy);
void or_e(struct gameboy * gameboy);
void or_h(struct gameboy * gameboy);
void or_l(struct gameboy * gameboy);
void or_hlp(struct gameboy * gameboy);
void or_a(struct gameboy * gameboy);
void cp_b(struct gameboy * gameboy);
void cp_c(struct gameboy * gameboy);
void cp_d(struct gameboy * gameboy);
void cp_e(struct gameboy * gameboy);
void cp_h(struct gameboy * gameboy);
void cp_l(struct gameboy * gameboy);
void cp_hlp(struct gameboy * gameboy);
void cp_a(struct gameboy * gameboy);

        //0xC0 - 0xCF
void ret_nz(struct gameboy * gameboy);
void pop_bc(struct gameboy * gameboy);
void jp_nz_nn(struct gameboy * gameboy, uint16_t nn);
void jp_nn(struct gameboy * gameboy, uint16_t nn);
void call_nz_nn(struct gameboy * gameboy, uint16_t nn);
void push_bc(struct gameboy * gameboy);
void add_a_n(struct gameboy * gameboy, uint8_t n);
void rst_0(struct gameboy * gameboy);
void ret_z(struct gameboy * gameboy);
void ret(struct gameboy * gameboy);
void jp_z_nn(struct gameboy * gameboy, uint16_t nn);
void ext_ops(struct gameboy * gameboy);
void call_z_nn(struct gameboy * gameboy, uint16_t nn);
void call_nn(struct gameboy * gameboy, uint16_t nn);
void adc_a_n(struct gameboy * gameboy, uint8_t n);
void rst_8(struct gameboy * gameboy);

//0xD0 - 0xDF
void ret_nc(struct gameboy * gameboy);
void pop_de(struct gameboy * gameboy);
void jp_nc_nn(struct gameboy * gameboy, uint16_t nn);
void call_nc_nn(struct gameboy * gameboy, uint16_t nn);
void push_de(struct gameboy * gameboy);
void sub_a_n(struct gameboy * gameboy, uint8_t n);
void rst_10(struct gameboy * gameboy);
void ret_c(struct gameboy * gameboy);
void reti(struct gameboy * gameboy);
void jp_c_nn(struct gameboy * gameboy, uint16_t nn);
void call_c_nn(struct gameboy * gameboy, uint16_t nn);
void sbc_a_n(struct gameboy * gameboy, uint8_t n);
void rst_18(struct gameboy * gameboy);

        //0xE0 - 0xEF
void ldh_n_a(struct gameboy * gameboy, uint8_t n);
void pop_hl(struct gameboy * gameboy);
void ldh_c_a(struct gameboy * gameboy);
void push_hl(struct gameboy * gameboy);
void and_n(struct gameboy * gameboy, uint8_t n);
void rest_20(struct gameboy * gameboy);
void add_sp_d(struct gameboy * gameboy);
void jp_hlp(struct gameboy * gameboy);
void ld_nnp_a(struct gameboy * gameboy, uint16_t nnp);
void xor_n(struct gameboy * gameboy, uint8_t n);
void rst_28(struct gameboy * gameboy);
 
  //0xF0 - 0xFF
void ldh_a_n(struct gameboy * gameboy, uint8_t n);
void pop_af(struct gameboy * gameboy);
void di(struct gameboy * gameboy);
void push_af(struct gameboy * gameboy);
void or_n(struct gameboy * gameboy, uint8_t n);
void rest_30(struct gameboy * gameboy);
void ldhl_sp_d(struct gameboy * gameboy);
void ld_sp_hl(struct gameboy * gameboy);
void ld_a_nnp(struct gameboy * gameboy, uint16_t nnp);
void ei(struct gameboy * gameboy);
void cp_n(struct gameboy * gameboy, uint8_t n);
void rst_38(struct gameboy * gameboy);

void undefined(struct gameboy * gameboy);

#endif
