# Virtual Machine simulating LC-3 architecture

### Learning project following:

https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf
https://www.jmeiners.com/lc3-vm/

# Notes

### Memory
- Making 65_536 memory locations (u16 max), each can hold 16 bits
  - Total of 128kb memory

### Registers
- For the CPU to work with data, is has to be in a register
- This VM (LC-3) has 10 registers, each register is 16 bits
- This is small, but programs work around this by:
  - Loading values from `memory` into registers
  - Calculating values into other registers
  - Storing the final results back into memory

- **8 general purpose registers** (R0 - R7)
  - Used to perform any program calculations
- **1 program counter register** (PC)
  - Unsigned int, stores the address of next instruction in memory to execute
- **Condition flags register** (COND)
  - Gives information about the previous calculation

### Instruction Set
- A command which tells the CPU to do some task (like add 2 numbers)
- Each instruction has both an `opcode` and a `set of parameters`
- `opcode`
  - Represents 1 task the CPU knows how to do
  - This vm has only 16 opcodes
  - Everything the computer can calculate is some sequence of these 16 instructions
- `set of parameters`
  - Provide inputs to the task being performed

- `Instruction`
  - 16 bits long
  - bits 0..=3   :  4 bits to store the opcode
  - bits 4..=16  :  12 bits to store parameters

- *Side note*
  - **Intel x86** has hundreds of instruction types
  - While others like **ARM** and **LC-3** have only a few
  - `RISCs`: Small instruction sets
    - Less complex, easier to write assembly for
    - Can require multiple instructions to do complex tasks
  - `CISCs`: Large instruction sets
    - More complex, many more things to know
    - Can do complex tasks in a single instruction


### Condition Flags
- `R_COND` register stores condition flags
  - Provides info about most recently executed calculation
- Allows programs to check logical conditions like `if (x > 0)`
- Each CPU has variety of condition flags to signal various situations
  - LC-3 uses only 3 condition flags



