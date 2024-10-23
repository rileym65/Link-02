#include "header.h"

void outputRcs() {
  int   i;
  FILE* file;
  byte  buffer[16];
  char  line[256];
  char  tmp[5];
  byte  count;
  word  outAddress;
  int   address;
  int   high;
  address = lowest;
  high = highest;
  outAddress = lowest;
  count = 0;
  file = fopen(outName, "w");
  while (address <= high) {
    if (map[address] == 1) {
      if (count == 0) outAddress = address;
      buffer[count++] = memory[address];
      if (count == 16) {
        strcpy(line,":");
        sprintf(tmp,"%04x",outAddress);
        strcat(line,tmp);
        for (i=0; i<count; i++) {
          sprintf(tmp," %02x",buffer[i]);
          strcat(line,tmp);
          }
        fprintf(file,"%s\n",line);
        count = 0;
        }
      }
    else if (count > 0) {
      strcpy(line,":");
      sprintf(tmp,"%04x",outAddress);
      strcat(line,tmp);
      for (i=0; i<count; i++) {
        sprintf(tmp," %02x",buffer[i]);
        strcat(line,tmp);
        }
      fprintf(file,"%s\n",line);
      count = 0;
      }
    address++;
    }
  if (count > 0) {
    strcpy(line,":");
    sprintf(tmp,"%04x",outAddress);
    strcat(line,tmp);
    for (i=0; i<count; i++) {
      sprintf(tmp," %02x",buffer[i]);
      strcat(line,tmp);
      }
    fprintf(file,"%s\n",line);
    count = 0;
    }
  if (startAddress != 0xffff) {
    sprintf(line,"@%04x",startAddress);
    fprintf(file,"%s\n",line);
    }
  fclose(file);
  }
