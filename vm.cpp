#include <cstdint>
#include <iostream>
#include <ostream>
#include "ops.h"
#include "memory.h"

// ============================
// ==== Condition Flags =======
// ============================
// provide info about most recently executed calculation
// can be used to check things like `if (x > 0)`
//
// LC-3 will only have 3 condition flags,
// indicating if prev. calc. was Positive, Negative, or Zero
enum ConditionFlag {
  FL_POS = 1 << 0, // Positive
  FL_ZR0 = 1 << 1, // Zero
  FL_NEG = 1 << 2, // Negative
};

// - Call with the register that was updated
// Any time a value is written to a register, we have to update the
// R_COND condition flags to indicate it's sign
// This is to be called AFTER the Instruction is executed whenever a reg is changed
void update_cond_flags(uint16_t write_reg) {
  if (reg[write_reg] == 0) {
    reg[R_COND] = FL_ZR0;
  } else if (reg[write_reg] >> 15) { // 1 in leftmost bit indicates negative
    reg[R_COND] = FL_NEG;
  } else {
    reg[R_COND] = FL_POS;
  }
}

// ============================
// ======= Procedure ==========
// ============================
// 1. Load one instruction from memory at the address of the PC register
// 2. Increment the PC register
// 3. Look at the opcode of the instruction to decide what instruction it should perform
// 4. Perform the instruction using the params in the instruction
// 5. Go back to step 1

int main(int argc, const char* argv[]) { 

  if (argc < 2) {
    // std::cout << "Usage: vm [image-file1] ..." << std::endl;
    // exit(2);
  }

  for (int i = 1; i < argc; ++i) {
    // if (!read_image(argv[i])) {
    //   std::cerr << "Failed to load image: " << argv[i] << std::endl;
    //   exit(1);
    // }
  }

  // set initial condition flag register to zero
  reg[R_COND] = FL_ZR0;

  // set program counter register to starting memory position
  // 0x3000 is default
  enum { PC_START = 0x3000 };
  reg[R_PC] = PC_START;

  exit(0);

  int running = 1;
  while (running) {
    // Get the instruction then increment PC register
    uint16_t instr = memory[reg[R_PC]++];
    // opcode : first 4 bits of instruction
    uint16_t opcode = static_cast<uint16_t>(instr >> 12);

    // something is broken with my clang formatter config
    switch (opcode) {
      case OP_ADD:
	add(instr);
	update_cond_flags(static_cast<uint16_t>((instr >> 9) & 0x7));
	break;
      case OP_AND:
	bitwise_and(instr);
	update_cond_flags(static_cast<uint16_t>((instr >> 9) & 0b0111));
      break;
      case OP_NOT:
	bitwise_complement(instr);
	update_cond_flags(static_cast<uint16_t>((instr >> 9) & 0b111));
      break;
      case OP_BR:
      break;
      case OP_JMP:
      break;
      case OP_JSR:
	jump_subr(instr);
      break;
      case OP_LD:
	load(instr);
	update_cond_flags(static_cast<uint16_t>((instr >> 9) & 0x7));
      break;
      case OP_LDI:
	load_indirect(instr);
	update_cond_flags(static_cast<uint16_t>((instr >> 9) & 0x7));
      break;
      case OP_LDR:
	load_base_offset(instr);
	update_cond_flags(static_cast<uint16_t>((instr >> 9) & 0x7));
      break;
      case OP_LEA:
	load_effective_addr(instr);
	update_cond_flags(static_cast<uint16_t>((instr >> 9) & 0x7));
      break;
      case OP_ST:
	store(instr);
      break;
      case OP_STI:
	store_indirect(instr);
      break;
      case OP_STR:
	store_base_offset(instr);
      break;
      case OP_TRAP:
	trap(instr);
      break;
      // unused
      case OP_RES:
      case OP_RTI:
	std::cerr << "Unused opcode" << std::endl;
	abort();
      default:
	std::cerr << "Invalid opcode" << std::endl;
	abort();
      break;
    }
  }
}

