#include "ops.h"
#include "memory.h"
#include <cstdint>

uint16_t sign_extend(uint16_t x, int bit_count) {
  if ((x >> (bit_count - 1)) & 1) {
    x |= static_cast<uint16_t>(0xFFFF << bit_count);
  }
  return x;
}

// ADD instruction
// - Accepts an instruction (u16)
// - Assigns result to DR
void add(uint16_t instr) {
  // don't need 4 most sig. bits, only last 12
  uint16_t designation_reg = static_cast<uint16_t>((instr >> 9) & 0x7);

  // SR1
  uint16_t sr1_reg = static_cast<uint16_t>((instr >> 6) & 0x7);

  // Check 5th bit for immediate mode or register mode
  uint16_t imm_flag = static_cast<uint16_t>((instr >> 5) & 1);

  if (imm_flag) {
    // last 5 bits is num to add (immediate mode)
    // sign-extended
    uint16_t imm5 = sign_extend(instr & 0x1F, 5);
    // store reg[sr1] + imm2 in DR
    reg[designation_reg] = reg[sr1_reg] + imm5;
  } else {
    // last 3 bits is sr2 register
    uint16_t sr2_reg = static_cast<uint16_t>(instr & 0x7);
    // store reg[sr1] + reg[sr2] in DR
    reg[designation_reg] = reg[sr1_reg] + reg[sr2_reg];
  }
}

// Load a value from a location in memory into a register
void load_indirect(uint16_t instr) {
  
  // 3 bits [11..=9] are designation register
  uint16_t dr = static_cast<uint16_t>((instr >> 9) & 0b0111);
  // 9 bits [8..=0], sign extended, added to Program Counter is the address
  uint16_t pc_offset9 = sign_extend((instr & 0b111111111), 9);
  // (main loop increments the program counter before executing instruction)
  // add to program counter & go to that address in memory
  // uint16_t addr = pc_offset9 + memory[reg[R_PC]];
  uint16_t addr = memory[pc_offset9 + reg[R_PC]];
  uint16_t value = memory[addr];

  // store that value into DR
  reg[dr] = value;
}

void bitwise_and(uint16_t instr) {
  // 4bit unused
  // 3bit DR
  uint16_t dr = static_cast<uint16_t>((instr >> 9) & 0b111);
  // 3bit reg of 1st operand
  uint16_t reg1 = static_cast<uint16_t>((instr >> 6) & 0b111);
  uint16_t val1 = reg[reg1];
  // 1bit mode
  uint16_t imm_mode = static_cast<uint16_t>((instr >> 5) & 1);
  if (imm_mode) {
    // 5 bits immediate value, sign extended
    uint16_t imm5 = sign_extend(instr & 0b11111, 5);
    reg[dr] = val1 & imm5;
  } else {
    // 2bit unised
    // last 3bits reg2
    uint16_t reg2 = static_cast<uint16_t>(instr & 0b111);
    uint16_t val2 = reg[reg2];
    reg[dr] = val1 & val2;
  }
}

void branch(uint16_t instr) {
  // 4bit unused
  // 3 condition bits, if any are set, branch to
  // 9bit PCOffset9
  // I think this is supposed to test the current COND flag
}

void jump(uint16_t instr) {
  // 4bit ignore
  // 3bit unused
  // 3bit base register or 111 RET
  // I shouldnt need to do anything different for JMP or RET
  // since RET = 111 which is REG7 register anyway right?
  uint16_t r = static_cast<uint16_t>((instr >> 6) & 0b111);
  // ?
  reg[R_PC] = reg[r];
}


void jump_subr(uint16_t instr) {
  // save (pre-incremented) PC in R7
  reg[R_R7] = reg[R_PC];
  // 1bit mode
  uint16_t mode = static_cast<uint16_t>((instr >> 11) & 1);

  if (mode) {
    // last 11 bits sign extended
    uint16_t pcoffset11 = sign_extend(instr & 0x7FF, 11);
    // load PC with val @ addr of pcoffset11 + incremented PC
    reg[R_PC] = memory[pcoffset11 + reg[R_PC]];
  } else {
    // 8..6bits are base reg
    uint16_t br = static_cast<uint16_t>((instr >> 6) & 0b111);
    // load PC with value in base_reg
    reg[R_PC] = reg[br];
  }
}

void load(uint16_t instr) {
  uint16_t dr = static_cast<uint16_t>((instr >> 9) & 0b111);
  uint16_t pcoffset9 = sign_extend(instr & 0b111111111, 9);

  // store val in mem at offset + pc in dr
  reg[dr] = memory[pcoffset9 + reg[R_PC]];
}

void load_base_offset(uint16_t instr) {
  uint16_t dr = static_cast<uint16_t>((instr >> 9) & 0b111);
  uint16_t br = static_cast<uint16_t>((instr >> 6) & 0b111);

  uint16_t offset6 = sign_extend(instr & 0b111111, 6);
  uint16_t addr = reg[br] + offset6;

  // store val in mem @ addr in dr
  reg[dr] = memory[addr];
}

void load_effective_addr(uint16_t instr) {
  uint16_t dr = static_cast<uint16_t>((instr >> 9) & 0b111);
  uint16_t addr = reg[R_PC] + sign_extend(instr & 0b111111111, 9);

  // stores addr in dr
  reg[dr] = addr;
}

void bitwise_complement(uint16_t instr) {
  uint16_t dr = static_cast<uint16_t>((instr >> 9) & 0b111);
  uint16_t sr = static_cast<uint16_t>((instr >> 6) & 0b111);

  // store bitwise complement of content in SR into DR
  reg[dr] = ~reg[sr];
}

void store(uint16_t instr) {
  uint16_t sr = static_cast<uint16_t>((instr >> 9) & 0b111);
  // contensdt of SR reg are stored in memory location
  // @ PCoffset9 sign extended + PC
  uint16_t addr = reg[R_PC] + sign_extend(instr & 0b111111111, 9);
  memory[addr] = reg[sr];
}

void store_indirect(uint16_t instr) {
  uint16_t sr = static_cast<uint16_t>((instr >> 9) & 0b111);
  uint16_t addr = reg[R_PC] + sign_extend(instr & 0b111111111, 9);
  // content of sr are stored in addr stored at  memory[addr]
  memory[memory[addr]] = reg[sr];
}

void store_base_offset(uint16_t instr) {
  uint16_t sr = static_cast<uint16_t>((instr >> 9) & 0b111);
  uint16_t br = static_cast<uint16_t>((instr >> 6) & 0b111);

  // contents of reg[sr] are stored in mem with addr of
  // sign_extend[6bit offset] + contents of br
  uint16_t addr = reg[br] + sign_extend(instr & 0b111111, 6);
  memory[addr] = reg[sr];
}

void trap(uint16_t instr) {
  // reg7 loaded with PC
  reg[R_R7] = reg[R_PC];

  // PC loaded w/ start addr of syscall specified by trapvector
  // that start addr is contained in mem location of trapvec 0 extended to 16bits
  uint16_t start_addr = static_cast<uint16_t>(((instr & 0xFF) << 8) >> 8);

  reg[R_PC] = memory[start_addr];
}

