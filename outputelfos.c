#include "header.h"

void outputElfos() {
  int file;
  word load;
  word size;
  word exec;
  char header[6];
  exec = startAddress;
  load = lowest;
  size = (highest-lowest) + 1;
  header[0] = (load >> 8) & 0xff;
  header[1] = load & 0xff;
  header[2] = (size >> 8) & 0xff;
  header[3] = size & 0xff;
  header[4] = (exec >> 8) & 0xff;
  header[5] = exec & 0xff;
  file = open(outName, O_WRONLY | O_CREAT | O_TRUNC|O_BINARY, 0666);
  write(file, header, 6);
  write(file, memory+lowest, (highest-lowest)+1);
  close(file);
  }
