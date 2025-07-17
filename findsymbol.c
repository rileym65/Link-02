#include "header.h"

int findSymbol(char* name, char *module) {
  int i;
  if (module == NULL) {
    for (i=0; i<numSymbols; i++)
      if (strcmp(symbols[i],name) == 0) return i;
    return -1;
    }
  for (i=0; i<numSymbols; i++)
    if (strcmp(symbols[i],name) == 0 &&
        strcmp(symbolModules[i],module) == 0) return i;
  return -1;
  }
