#include "header.h"

void writeMem(word address, word value) {
  if (addressMode == 'L') {
    memory[address] = value & 0xff;
    memory[address+1] = (value >> 8) & 0xff;
    }
  else {
    memory[address+1] = value & 0xff;
    memory[address] = (value >> 8) & 0xff;
    }
  map[address] = 1;
  map[address+1] = 1;
  }
