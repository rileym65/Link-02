#include "header.h"

int findSymbol(char* name) {
  int i;
  for (i=0; i<numSymbols; i++)
    if (strcmp(symbols[i],name) == 0) return i;
  return -1;
  }
