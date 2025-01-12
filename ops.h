#ifndef OPS_H
#define OPS_H
#include <cstdint>

// ============================
// ==== Instruction Set =======
// ============================
// Each instruction has both an
// Opcode     : Indicates the task to perform
// Parameters : Inputs to the task being performed
// -- Opcodes --
// Each opcode represents 1 task the CPU "knows how to do"
// LC3 has only 16 opcodes
// -- Instructions
// Everything the VM can do is some sequence of these instructions
// 1 instruction = 16 bits
// [0..=3]  : 4 bits for the Opcode
// [4..=16] : 12 bits for parameters
enum Instruction {
  OP_BR = 0, /* branch */
  OP_ADD,    /* add  */
  OP_LD,     /* load */
  OP_ST,     /* store */
  OP_JSR,    /* jump register */
  OP_AND,    /* bitwise and */
  OP_LDR,    /* load register */
  OP_STR,    /* store register */
  OP_RTI,    /* unused */
  OP_NOT,    /* bitwise not */
  OP_LDI,    /* load indirect */
  OP_STI,    /* store indirect */
  OP_JMP,    /* jump */
  OP_RES,    /* reserved (unused) */
  OP_LEA,    /* load effective address */
  OP_TRAP    /* execute trap */
};

// - Pads `x` with 16 - `bit_count` bits with with 0's (positive) or 1's (negative)
// - So if x is 5 bits, use `bit_count = 5`
uint16_t sign_extend(uint16_t x, int bit_count);

// takes 2 numbers, adds them together, stores result in a register
// Each add instruction (16 bits)
// ------- First 10 bits are always the same
// 0001    4bits : Opcode, ADD is 0001
// 000     3bits : Designation Register, where the result is stored
// 000     3bits : SR1, register containing first number to add
// ------- Last 6 bits
// --- If Register mode
// 0       1bit  : 0 indicates it's register mode
// 00      2bits : Unused
// 000     3bits : SR2, register containing the second number to add
// --- If Immediate mode
// 1       1bit  : 1 indicates it's immediate mode
// 00000   5bits : Holds the second number to add, but max is 2^5 (32)
// -----------------------
// If register mode,
// - obtain second number from SR2 register provided
// If Immediate mode, 
// - Must sign-extend the 5bit value to 16bits (to match SR1) before adding
// --- Fills in 0's for positive nums, 1's for negative nums
void add(uint16_t instr);

// Load a value from a location in memory into a register
// 1010       : 4bits, indicates LDI instruction
// 000        : 3bits, Designation register
// 000000000  : 9bits, PCoffset9
// - PCoffset9 - 
// Immediate value stored in instruction
// sign extend these 9 bits to 16,
// add that value to the incremented Program Counter (R_PC) register
// What is stored in memory at this address is the addr of the data to load into DR
void load_indirect(uint16_t instr);

// bitwise logical AND
// 0101     : 4bit instruction
// 000      : 3bit DR
// 000      : 3bit register of 1st operand
// 0        : 1bit, 0 = register mode, 1 = immediate mode
// -- Register mode
// 00       : 2bit unused
// 000      : 3bit register of 2nd operand
// -- Immediate mode
// 00000    : 5bit value of 2nd operand
void bitwise_and(uint16_t instr);

// Conditional branch
// 0000     : 4bit instruction
// 0        : 1bit N condition
// 0        : 1bit Z condition
// 0        : 1bit P condition
// 000000000: 9bit PCoffset9
void branch(uint16_t instr);

// 1100     : 4bit instruction
// 000      : 3bit unused
// 000      : Base Register, if 111 RET
// ...      : 6bit unused
// -- Base Register not 111
// Unconditionally jump to the location specified by
// the contents of BR, ex. JMP REG2 ; PC <- REG2
// (set PC to REG2)
// -- If 111
// RET, special case of JMP instruction
// Load PC with contents of REG7, which is link
// back to the instr. following the subroutine call instr.
void jump(uint16_t instr);

// 0100      : 4bit instr
// 0         : 1bit mode
// ---
// - mode=1 -
// ...       : 11bit PCoffset 11
// - mode=0 -
// 00        : 2bit unused
// 000       : 3bit base reg
// ...       : 6bit unused
// ----
// 1. Incremented PC saved in reg7
// 2. PC loaded with addr: base_reg or PCoffset11
// If PCoffset11, addr is sign extended PCoffset11 + PC
void jump_subr(uint16_t instr);

// 4bit instr
// 3bit DR
// 9bit PCoffset9
void load(uint16_t instr);

// 0110   4bit instr
// 000    3bit DR
// 000    3bit BaseR
// ...    6bit offset6
void load_base_offset(uint16_t instr);

// 1110   4bit instr
// 000    3bit DR
// ...    9bit PCoffset
void load_effective_addr(uint16_t instr);

// 1001   4bit instr
// 000    3bit DR
// 000    3bit SR
// ...    6bit ignored?
void bitwise_complement(uint16_t instr);

// 0011   4bit instr
// 000    3bit SR
// ...    9bit PCoffset
void store(uint16_t instr);

// 1011   4bit instr
// 000    3bit SR
// ...    9bit PCoffset
void store_indirect(uint16_t instr);

// 0111   4bit instr
// 000    3bit SR
// 000    3bit BR
// ...    6bit offset
void store_base_offset(uint16_t instr);

// 1111   4bit instr
// 0000   4bit ignord
// ...    8bit trapvect
void trap(
  uint16_t instr,
  void (*upd_cond_flags)(uint16_t),
  int* running
);

// Read single char from keyboard, not echoed to console
// It's ASCII code is copied into R0, the high 8 bits of R0 are cleared
void trap_getc(void (*upd_cond_flags)(uint16_t));

// Write a character in R0 (lower 8 bits) to console
void trap_out();

// starting at address in r0, write each value in memory
// to stdout until terminator is reached
void trap_puts();

// Print prompt on screen & read a single char from keyboard
// character is echoed onto console, then
// it's ASCII code is copied into R0
// high 8 bits of R0 are cleared
void trap_in(void (*upd_cond_flags)(uint16_t));


// write string of ASCII characters to console
// starts with address in R0
// then each memory address contains two 8bit chars
// print lower 8 bits first, then higher 8 bits
// if odd num chars, last one will have x00 in higher 8 bits
// finishes at occurence of x0000 in a memory location
void trap_puts_p();


// halt execution & print message on console
void trap_halt(int* running);







#endif // !OPS_H
