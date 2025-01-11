#ifndef MEMORY_H
#define MEMORY_H
#include <cstdint>

// ============================
// ========= Memory ===========
// ============================
// max value of unsigned 16bit int: 65_536
#define MEMORY_MAX (1 << 16)

// 65_536 memory locations, each location can store a 16bit value (128KB total)
extern uint16_t memory[MEMORY_MAX];

// ============================
// ========= Registers ========
// ============================
// 10 registers, 16bits each
enum Register {
  // 8 general purpose registers
  // used to perform calculations
  R_R0 = 0,
  R_R1, R_R2, R_R3, R_R4, R_R5, R_R6, R_R7,
  // Program counter register
  // uint which is address of next memory instruction to execute
  R_PC,
  // Condition flags register
  // gives information about the previous calculation
  R_COND,
  // used as number of Registers since Register::R_COUNT = 10
  R_COUNT
};

// Register storage, length 10
// each idx can be accessed via reg[Register::R_R2] etc since enum gives int
// this is a cool way to name each index in reg
extern uint16_t reg[Register::R_COUNT];


#endif // !MEMORY_H
