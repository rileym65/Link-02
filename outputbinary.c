#include "header.h"

void outputBinary() {
  int file;
  file = open(outName, O_WRONLY | O_CREAT | O_TRUNC|O_BINARY, 0666);
  write(file, memory+lowest, (highest-lowest)+1);
  close(file);
  }
