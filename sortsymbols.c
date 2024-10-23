#include "header.h"

void sortSymbols() {
  char flag;
  int  i;
  word t;
  char *c;
  flag = 1;
  while (flag == 1) {
    flag = 0;
    for (i=0; i<numSymbols-1; i++)
      if (values[i] > values[i+1]) {
        c = symbols[i]; symbols[i] = symbols[i+1]; symbols[i+1] = c;
        t = values[i];  values[i] = values[i+1]; values[i+1] = t;
        flag = 1;
        }
    }
  }
