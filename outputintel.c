#include "header.h"

void outputIntel() {
  int   i;
  FILE* file;
  byte  buffer[16];
  char  line[256];
  char  tmp[5];
  byte  count;
  word  outAddress;
  word  checksum;
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
        sprintf(tmp,"%02x",count);
        strcat(line,tmp);
        checksum = count;
        sprintf(tmp,"%04x",outAddress);
        strcat(line,tmp);
        checksum += ((outAddress >> 8) & 0xff);
        checksum += (outAddress & 0xff);
        strcat(line,"00");
        for (i=0; i<count; i++) {
          sprintf(tmp,"%02x",buffer[i]);
          strcat(line,tmp);
          checksum += buffer[i];
          }
        checksum = (checksum ^ 0xffff) + 1;
        sprintf(tmp,"%02x",checksum & 0xff);
        strcat(line,tmp);
        fprintf(file,"%s\n",line);
        count = 0;
        }
      }
    else if (count > 0) {
      strcpy(line,":");
      sprintf(tmp,"%02x",count);
      strcat(line,tmp);
      checksum = count;
      sprintf(tmp,"%04x",outAddress);
      strcat(line,tmp);
      checksum += ((outAddress >> 8) & 0xff);
      checksum += (outAddress & 0xff);
      strcat(line,"00");
      for (i=0; i<count; i++) {
        sprintf(tmp,"%02x",buffer[i]);
        strcat(line,tmp);
        checksum += buffer[i];
        }
      checksum = (checksum ^ 0xffff) + 1;
      sprintf(tmp,"%02x",checksum & 0xff);
      strcat(line,tmp);
      fprintf(file,"%s\n",line);
      count = 0;
      }
    address++;
    }
  if (count > 0) {
    strcpy(line,":");
    sprintf(tmp,"%02x",count);
    strcat(line,tmp);
    checksum = count;
    sprintf(tmp,"%04x",outAddress);
    strcat(line,tmp);
    checksum += ((outAddress >> 8) & 0xff);
    checksum += (outAddress & 0xff);
    strcat(line,"00");
    for (i=0; i<count; i++) {
      sprintf(tmp,"%02x",buffer[i]);
      strcat(line,tmp);
      checksum += buffer[i];
      }
    checksum = (checksum ^ 0xffff) + 1;
    sprintf(tmp,"%02x",checksum & 0xff);
    strcat(line,tmp);
    fprintf(file,"%s\n",line);
    count = 0;
    }
  if (startAddress != 0xffff) {
    sprintf(line,":040000050000%02x%02x",(startAddress >> 8) & 0xff,startAddress & 0xff);
    checksum = 4 + 5 + ((startAddress >> 8) & 0xff) + (startAddress & 0xff);
    checksum = (checksum ^ 0xffff) + 1;
    sprintf(tmp,"%02x",checksum & 0xff);
    strcat(line,tmp);
    fprintf(file,"%s\n",line);
    }
  fprintf(file,":00000001FF\n");
  fclose(file);
  }
