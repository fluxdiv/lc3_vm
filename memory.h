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


// Memory Mapped Registers: Registers not accessible from normal register table
// - A special address is reserved for them in memory
// - To read/write to them, you read/write to their memory location
enum MemMapRegister {
  // Keyboard status register
  // - Indicates if a key has been pressed
  MMR_KBSR = 0xFE00,
  // Keyboard data register
  // - Identifies what key was pressed
  MMR_KBDR = 0xFE02,
};

// Now that there are memorymapped registers, the way I access memory has to change
// I can't read/write directly to memory, because I could
// inadvertently modify the MemMapRegisters
// So, I need getter/setter functions
//
// When memory is read from Keyboard Status Reg MMR_KBSR,
// the getter will check the keyboard & update both keyboard registers

// Updates memory[address] with val
void mem_write(uint16_t address, uint16_t val);

// -If address is keyboard status register MMR_KBSR:
// -- If a key has been pressed:
//   --- Set MMR_KBSR to 1 << 15 (toggle to "true")
//   --- Set MMR_KBDR to the key pressed (Keyboard Data Register)
// -- No key was pressed:
//   --- Set MMR_KBSR to 0 (toggle to "false")
// -If any other address:
// -- Just return `memory[address]`
uint16_t mem_read(uint16_t address);


void disable_input_buffering();

void restore_input_buffering();


int read_image(const char* image_path, uint16_t& pc);

#endif // !MEMORY_H
