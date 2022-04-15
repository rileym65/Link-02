#define MAIN

#include "header.h"

char* getHex(char* line, word *value) {
  *value = 0;
  while ((*line >= '0' && *line <= '9') ||
         (*line >= 'a' && *line <= 'f') ||
         (*line >= 'A' && *line <= 'F')) {
    if (*line >= '0' && *line <= '9')
      *value = (*value << 4) + (*line - '0');
    else if (*line >= 'a' && *line <= 'f')
      *value = (*value << 4) + (*line - 87);
    else if (*line >= 'A' && *line <= 'F')
      *value = (*value << 4) + (*line - 65);
    line++;
    }
  return line;
  }

int findSymbol(char* name) {
  int i;
  for (i=0; i<numSymbols; i++)
    if (strcmp(symbols[i],name) == 0) return i;
  return -1;
  }

word readMem(word address) {
  word ret;
  if (addressMode == 'L')
    ret = memory[address] + (memory[address+1] << 8);
  else
    ret = memory[address+1] + (memory[address] << 8);
  return ret;
  }

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

void addReference(char* name, word value, char typ) {
  numReferences++;
  if (numReferences == 1) {
    references = (char**)malloc(sizeof(char*));
    addresses = (word*)malloc(sizeof(word));
    types = (char*)malloc(sizeof(char));
    }
  else {
    references = (char**)realloc(references,sizeof(char*)*numReferences);
    addresses = (word*)realloc(addresses,sizeof(word)*numReferences);
    types = (char*)realloc(types,sizeof(char)*numReferences);
    }
  references[numReferences-1] = (char*)malloc(strlen(name) + 1);
  strcpy(references[numReferences-1], name);
  addresses[numReferences-1] = value;
  types[numReferences-1] = typ;
  }

int loadFile(char* filename) {
  int   i;
  char  buffer[1024];
  char  token[256];
  int   pos;
  FILE *file;
  word  value;
  word  addr;
  char *line;
  printf("Linking: %s\n",filename);
  inProc = 0;
  offset = 0;
  file = fopen(filename,"r");
  if (file == NULL) {
    printf("Could not open input file: %s\n",filename);
    return -1;
    }
  while (fgets(buffer, 1023, file) != NULL) {
    line = buffer;
    if (strncmp(line,".big",4) == 0) addressMode = 'B';
    else if (strncmp(line,".little",7) == 0) addressMode = 'L';
    else if (*line == ':') {
      line++;
      line = getHex(line, &address);
      if (inProc) address += offset;
      while (*line != 0) {
        while (*line > 0 && *line <= ' ') line++;
        if (*line != 0) {
          line = getHex(line, &value);
          if (address < lowest) lowest = address;
          if (address > highest) highest = address;
          if (map[address] != 0) {
            printf("Error: Collision at %04x\n",address);
            }
          memory[address] = value & 0xff;
          map[address++] = 1;
          }
        }
      }
    else if (*line == '@') {
      line=buffer+1;
      getHex(line, &startAddress);
      }
    else if (*line == '+') {
      line++;
      line = getHex(line, &addr);
      value = readMem(addr+offset);
      value += offset;
      writeMem(addr+offset, value);
      }
    else if (*line == '=') {
      line++;
      pos = 0;
      while (*line != 0 && *line > ' ') token[pos++] = *line++;
      token[pos] = 0;
      while (*line == ' ') line++;
      getHex(line, &value);
      if (inProc) value += offset;
      for (i=0; i<numSymbols; i++)
        if (strcmp(token, symbols[i]) == 0) {
          printf("Error: Duplicate symbol: %s\n",token);
          fclose(file);
          return -1;
          }
      numSymbols++;
      if (numSymbols == 1) {
        symbols = (char**)malloc(sizeof(char*));
        values = (word*)malloc(sizeof(word));
        }
      else {
        symbols = (char**)realloc(symbols,sizeof(char*)*numSymbols);
        values = (word*)realloc(values,sizeof(word)*numSymbols);
        }
      symbols[numSymbols-1] = (char*)malloc(strlen(token) + 1);
      strcpy(symbols[numSymbols-1], token);
      values[numSymbols-1] = value;
      }
    else if (*line == '?') {
      line++;
      pos = 0;
      while (*line != 0 && *line > ' ') token[pos++] = *line++;
      token[pos] = 0;
      while (*line == ' ') line++;
      getHex(line, &value);
      if (inProc) value += offset;
      addReference(token, value, 'W');
      }
    else if (*line == '/') {
      line++;
      pos = 0;
      while (*line != 0 && *line > ' ') token[pos++] = *line++;
      token[pos] = 0;
      while (*line == ' ') line++;
      getHex(line, &value);
      if (inProc) value += offset;
      addReference(token, value, 'H');
      }
    else if (*line == '\\') {
      line++;
      pos = 0;
      while (*line != 0 && *line > ' ') token[pos++] = *line++;
      token[pos] = 0;
      while (*line == ' ') line++;
      getHex(line, &value);
      if (inProc) value += offset;
      addReference(token, value, 'L');
      }
    else if (*line == '{') {
      line++;
      pos = 0;
      while (*line != 0 && *line > ' ') token[pos++] = *line++;
      token[pos] = 0;
      value = address;
      for (i=0; i<numSymbols; i++)
        if (strcmp(token, symbols[i]) == 0) {
          printf("Error: Duplicate symbol: %s\n",token);
          fclose(file);
          return -1;
          }
      inProc = -1;
      offset = address;
      numSymbols++;
      if (numSymbols == 1) {
        symbols = (char**)malloc(sizeof(char*));
        values = (word*)malloc(sizeof(word));
        }
      else {
        symbols = (char**)realloc(symbols,sizeof(char*)*numSymbols);
        values = (word*)realloc(values,sizeof(word)*numSymbols);
        }
      symbols[numSymbols-1] = (char*)malloc(strlen(token) + 1);
      strcpy(symbols[numSymbols-1], token);
      values[numSymbols-1] = value;
      }
    else if (*line == '}') {
      inProc = 0;
      offset = 0;
      }
    }
  fclose(file);
  return 0;
  }

int link() {
  int i;
  int s;
  int errors;
  word v;
  errors=0;
  for (i=0; i<numReferences; i++) {
    s = findSymbol(references[i]);
    if (s < 0) {
      printf("Symbol not found: %s\n",references[i]);
      errors++;
      }
    else {
      address = addresses[i];
      if (types[i] == 'W') {
        v = readMem(address) + values[s];
        writeMem(address, v);
        }
      if (types[i] == 'H') {
        v = memory[address] + (values[s] >> 8);
        memory[address] = v & 0xff;
        }
      if (types[i] == 'L') {
        v = memory[address] + values[s];
        memory[address] = v & 0xff;
        }
      }
    }
  if (errors != 0) return -1;
  return 0;
  }

void outputBinary() {
  int file;
  file = open(outName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  write(file, memory+lowest, (highest-lowest)+1);
  close(file);
  }

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
  file = open(outName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  write(file, header, 6);
  write(file, memory+lowest, (highest-lowest)+1);
  close(file);
  }

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

int main(int argc, char **argv) {
  int   i;
  char *pchar;
  printf("Link/02 v1.0\n");
  printf("By Michael H. Riley\n\n");
  lowest = 0xffff;
  highest = 0x0000;
  numObjects = 0;
  startAddress = 0xffff;
  showSymbols = 0;
  numSymbols = 0;
  numReferences = 0;
  addressMode = 'L';
  strcpy(outName,"");
  outMode = BM_BINARY;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i], "-b") == 0) outMode = BM_BINARY;
    else if (strcmp(argv[i], "-c") == 0) outMode = BM_CMD;
    else if (strcmp(argv[i], "-e") == 0) outMode = BM_ELFOS;
    else if (strcmp(argv[i], "-i") == 0) outMode = BM_INTEL;
    else if (strcmp(argv[i], "-h") == 0) outMode = BM_RCS;
    else if (strcmp(argv[i], "-s") == 0) showSymbols = -1;
    else if (strcmp(argv[i], "-be") == 0) addressMode = 'B';
    else if (strcmp(argv[i], "-le") == 0) addressMode = 'L';
    else if (strcmp(argv[i], "-o") == 0) {
      i++;
      strcpy(outName, argv[i]);
      }
    else {
      numObjects++;
      if (numObjects == 1)
        objects = (char**)malloc(sizeof(char*));
      else
        objects = (char**)realloc(objects,sizeof(char*)*numObjects);
      objects[numObjects-1] = (char*)malloc(strlen(argv[i])+1);
      strcpy(objects[numObjects-1],argv[i]);
      }
    }
  if (numObjects == 0) {
    printf("No object files specified\n");
    exit(1);
    }
  if (strlen(outName) == 0) {
    strcpy(outName, objects[0]);
    pchar = strchr(outName, '.');
    if (pchar != NULL) *pchar = 0;
    if (outMode == BM_BINARY) strcat(outName, ".bin");
    if (outMode == BM_CMD) strcat(outName, ".cmd");
    if (outMode == BM_ELFOS) strcat(outName, ".elfos");
    if (outMode == BM_INTEL) strcat(outName, ".intel");
    if (outMode == BM_RCS) strcat(outName, ".hex");
    }
  for (i=0; i<65536; i++) {
    memory[i] = 0;
    map[i] = 0;
    }
  address = 0;
  for (i=0; i<numObjects; i++)
    if (loadFile(objects[i]) < 0) {
      printf("Errors: aborting linke\n");
      exit(1);
      }
  if (link() < 0) {
    printf("Errors during link.  Aborting output\n");
    exit(1);
    }
  else {
    printf("Writing: %s\n",outName);
    switch (outMode) {
      case BM_BINARY: outputBinary(); break;
      case BM_ELFOS : outputElfos(); break;
      case BM_INTEL : outputIntel(); break;
      case BM_RCS   : outputRcs(); break;
      }
    }
  printf("Lowest address : %04x\n",lowest);
  printf("Highest address: %04x\n",highest);
  if (startAddress != 0xffff)
    printf("Start address  : %04x\n",startAddress);
  if (showSymbols) {
    for (i=0; i<numSymbols; i++)
      printf("%-20s %04x\n",symbols[i], values[i]);
    }
  printf("\n");
  }

