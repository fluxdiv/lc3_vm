#include "memory.h"
#include <cstddef>
#include <cstdio>
#include <sys/select.h>
#include <stdio.h>
#include <stdint.h>
// #include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>


uint16_t memory[MEMORY_MAX];

uint16_t reg[Register::R_COUNT];

uint16_t swap16(uint16_t);
int read_image(const char* image_path);

// Memory getter/setter

void mem_write(uint16_t address, uint16_t val) {
  memory[address] = val;
}

uint16_t check_key();

uint16_t mem_read(uint16_t address) {
  if (address == MemMapRegister::MMR_KBSR) {
    if (check_key()) {
      memory[MMR_KBSR] = (1 << 15);
      memory[MMR_KBDR] = getchar();
    } else {
      memory[MMR_KBSR] = 0;
    }
  }
  return memory[address];
}

// Accesses keyboard?
uint16_t check_key() {
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

struct termios original_tio;

void disable_input_buffering() {
  tcgetattr(STDIN_FILENO, &original_tio);
  struct termios new_tio = original_tio;
  new_tio.c_lflag &= ~ICANON & ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering() {
  tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}




// The first 16 bits of the program bin file specify the address
// in memory where the program should start (The "Origin")
//
// This is read first, after which the rest of the data
// can be read from the file into memory starting at the origin addr
void read_image_file(FILE* file, uint16_t& pc) {
  // 16bit origin tells us where in memory to place the image
  uint16_t origin;
  // read the first 16bits of file into origin
  fread(&origin, sizeof(origin), 1, file);
  // swap from little-endian to big-endian
  origin = swap16(origin);
  pc = origin;

  // We know max file size so we only need 1 fread
  // the size between origin & end of memory
  uint16_t max_read = MEMORY_MAX - origin;

  // Pointer to origin offset in memory,
  // which is where we place the image file
  uint16_t* p = memory + origin;
  
  // writes `max_read` objects, each being 16bits,
  // and the first is written at `p` which is the origin
  // location in memory
  // returns the # of objects read successfully
  size_t read = fread(p, sizeof(uint16_t), max_read, file);

  // endian swap each value that was read
  while (read-- > 0) {
    *p = swap16(*p);
    ++p;
  }
}

// LC3 programs are big-endian, but most modern computers are little-endian
// so we need to swap each uint16 that's loaded
// little endian  : 1st byte is least significant 
// big-endian     : 1st byte is most significant
uint16_t swap16(uint16_t x) {
  // what is the point of this
  // let x    :   1100-1100   0011-0011
  // (x << 8) :   0011-0011   0000-0000
  // (x >> 8) :   0000-0000   1100-1100
  // x<<8|x>>8:   0011-0011   1100-1100
  // It just swaps 1st byte & 2nd byte
  return (x << 8) | (x >> 8);
}

// helper to call read_image_file
int read_image(const char* image_path, uint16_t& pc) {
  FILE* file = fopen(image_path, "rb");
  if (!file) { return 0; };
  read_image_file(file, pc);
  fclose(file);
  return 1;
}




