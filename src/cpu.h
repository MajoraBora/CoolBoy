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

// 0x00 - 0x0F
void nop(void); //0
void ld_bc_nn(void); //1
void ld_bc_a(void); //2
void inc_bc(void); //3
void inc_b(void); //4
void dec_b(void); //5
void ld_b_n(void); //6
void rlc_a(void); //7
void ld_nn_sp(void); //8
void add_hl_bc(void); //9
void ld_a_bcp(void); //A
void dec_bc(void); //B
void inc_c(void); //C
void dec_c(void); //D
void ld_c_n(void); //E
void rrc_a(void); //F

// 0x10 - 0x1F
void stop(void); //0x10
void ld_de_nn(void); //0x11
void ld_dep_a(void); //0x12
void inc_de(void); //0x13
void inc_d(void); //0x14
void dec_d(void); //0x15
void ld_d_n(void); //0x16
void rl_a(void); //0x17
void jr_n(void); //0x18
void add_hl_de(void); //0x19
void ld_a_dep(void); //0x1A
void dec_de(void); //0x1B
void inc_e(void); //0x1C
void dec_e(void); //0x1D
void ld_e_n(void); //0x1E
void rr_a(void); //0x1F

//0x20 - 0x2F
void jr_nz_n(void);
void ld_hl_nn(void);
void ldi_hlp_a(void);
void inc_hl(void);
void inc_h(void);
void dec_h(void);
void ld_h_n(void);
void daa(void);
void jr_z_n(void);
void add_hl_hl(void);
void ldi_a_hlp(void);
void dec_hl(void);
void ld_l_n(void);
void cpl(void);

// 0x30 - 0x3F
void jr_nc_n(void);
void ld_sp_nn(void);
void ldd_hlp_a(void);
void inc_sp(void);
void inc_hl(void);
void dec_hl(void);
void ld_hlp_n(void);
void scf(void);
void jr_c(void);
void add_hl_sp(void);
void ldd_a_hl(void);
void dec_sp(void);
void inc_a(void);
void dec_a(void);
void ld_a_n(void);
void ccf(void);

        // 0x40 - 0x4F
void ld_b_b(void);
void ld_b_c(void);
void ld_b_d(void);
void ld_b_e(void);
void ld_b_h(void);
void ld_b_l(void);
void ld_b_hlp(void);
void ld_b_a(void);
void ld_c_b(void);
void ld_c_c(void);
void ld_c_d(void);
void ld_c_e(void);
void ld_c_h(void);
void ld_c_l(void);
void ld_c_hlp(void);
void ld_c_a(void);

//0x50 - 0x5F
void ld_d_b(void);
void ld_d_c(void);
void ld_d_d(void);
void ld_d_e(void);
void ld_d_h(void);
void ld_d_l(void);
void ld_d_hlp(void);
void ld_d_a(void);
void ld_e_b(void);
void ld_e_c(void);
void ld_e_d(void);
void ld_e_e(void);
void ld_e_h(void);
void ld_e_l(void);
void ld_e_hlp(void);
void ld_e_a(void);

        //0x60 - 0x6F
void ld_h_b(void);
void ld_h_c(void);
void ld_h_d(void);
void ld_h_e(void);
void ld_h_h(void);
void ld_h_l(void);
void ld_h_hlp(void);
void ld_h_a(void);
void ld_l_b(void);
void ld_l_c(void);
void ld_l_d(void);
void ld_l_e(void);
void ld_l_h(void);
void ld_l_l(void);
void ld_l_hlp(void);
void ld_l_a(void);

//0x70-0x7F
void ld_hlp_b(void);
void ld_hlp_c(void);
void ld_hlp_d(void);
void ld_hlp_e(void);
void ld_hlp_h(void);
void ld_hlp_l(void);
void halt(void);
void ld_hlp_a(void);
void ld_a_b(void);
void ld_a_c(void);
void ld_a_d(void);
void ld_a_e(void);
void ld_a_h(void);
void ld_a_l(void);
void ld_a_hlp(void);
void ld_a_a(void);

        //0x80 - 0x8F
void add_a_b(void);
void add_a_c(void);
void add_a_d(void);
void add_a_e(void);
void add_a_h(void);
void add_a_l(void);
void add_a_hlp(void);
void add_a_a(void);
void adc_a_b(void);
void adc_a_c(void);
void adc_a_d(void);
void adc_a_e(void);
void adc_a_h(void);
void adc_a_l(void);
void adc_a_hlp(void);
void adc_a_a(void);

 //0x90 - 0x9F
void sub_a_b(void);
void sub_a_c(void);
void sub_a_d(void);
void sub_a_e(void);
void sub_a_h(void);
void sub_a_l(void);
void sub_a_hlp(void);
void sub_a_a(void);
void sbc_a_b(void);
void sbc_a_c(void);
void sbc_a_d(void);
void sbc_a_e(void);
void sbc_a_h(void);
void sbc_a_l(void);
void sbc_a_hlp(void);
void sbc_a_a(void);

        //0xA0 - 0xAF
void and_b(void);
void and_c(void);
void and_d(void);
void and_e(void);
void and_h(void);
void and_l(void);
void and_hlp(void);
void and_a(void);
void xor_b(void);
void xor_c(void);
void xor_d(void);
void xor_e(void);
void xor_h(void);
void xor_l(void);
void xor_hlp(void);
void xor_a(void);

//0xB0 - 0xBF
void or_b(void);
void or_c(void);
void or_d(void);
void or_e(void);
void or_h(void);
void or_l(void);
void or_hlp(void);
void or_a(void);
void cp_b(void);
void cp_c(void);
void cp_d(void);
void cp_e(void);
void cp_h(void);
void cp_l(void);
void cp_hlp(void);
void cp_a(void);

        //0xC0 - 0xCF
void ret_nz(void);
void pop_bc(void);
void jp_nz(void);
void jp_nn(void);
void call_nz_nn(void);
void push_bc(void);
void add_a_n(void);
void rst_0(void);
void ret_z(void);
void ret(void);
void jp_z_nn(void);
void ext_ops(void);
void call_z_nn(void);
void call_nn(void);
void adc_a_n(void);
void rst_8(void);

//0xD0 - 0xDF
void ret_nc(void);
void pop_de(void);
void jp_nc(void);

void call_nc_nn(void);
void push_de(void);
void sub_a_n(void);
void rst_10(void);
void ret_c(void);
void reti(void);
void jp_c_nn(void);
void call_c_nn(void);
void sbc_a_n(void);
void rst_18(void);

        //0xE0 - 0xEF
void ldh_n_a(void);
void pop_hl(void);
void ldh_c_a(void);
void push_hl(void);
void and_n(void);
void rest_20(void);
void add_sp_d(void);
void jp_hlp(void);
void ld_nn_a(void);
void xor_n(void);
void rst_28(void);
 
  //0xF0 - 0xFF
void ldh_a_n(void);
void pop_af(void);
void di(void);
void push_af(void);
void or_n(void);
void rest_30(void);
void ldhl_sp_d(void);
void ld_sp_hl(void);
void ld_a_nn(void);
void ei(void);
void cp_n(void);
void rst_38(void);

void undefined(void);


#endif
