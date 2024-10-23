#include "header.h"

word readMem(word address) {
  word ret;
  if (addressMode == 'L')
    ret = memory[address] + (memory[address+1] << 8);
  else
    ret = memory[address+1] + (memory[address] << 8);
  return ret;
  }
