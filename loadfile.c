#include "header.h"

int loadFile(char* filename) {
  int   i;
  int   j;
  char  buffer[1024];
  char  token[256];
  char  path[2048];
  int   pos;
  int   flag;
  FILE *file;
  word  value;
  word  addr;
  word  lofs;
  word  low;
  word  dest;
  char *line;
  if (libScan == 0) printf("Linking: %s\n",filename);
  inProc = 0;
  offset = 0;
  file = fopen(filename,"r");
  if (file == NULL) {
    if (libScan != 0) {
      strcpy(buffer, LIBPATH);
      strcat(buffer, filename);
      file = fopen(buffer, "r");
      if (file == NULL) {
        i = 0;
        while (i < numLibPath) {
          strcpy(path, libPath[i]);
          if (path[strlen(path)-1] != '/') strcat(path,"/");
          strcat(path, filename);
          file = fopen(path, "r");
          if (file != NULL) i = numLibPath;
          i++;
          }
        if (file == NULL) {
          printf("Could not open library file: %s\n",filename);
          return -1;
          }
        }
      }
    else {
      i = 0;
      while (i < numIncPath) {
        strcpy(path, incPath[i]);
        if (path[strlen(path)-1] != '/') strcat(path,"/");
        strcat(path, filename);
        file = fopen(path, "r");
        if (file != NULL) i = numIncPath;
        i++;
        }
      if (file == NULL) {
        printf("Could not open input file: %s\n",filename);
        return -1;
        }
      }
    }
  while (fgets(buffer, 1023, file) != NULL) {
    line = buffer;
    if (strncmp(line,".big",4) == 0) addressMode = 'B';
    else if (strncmp(line,".little",7) == 0) addressMode = 'L';
    else if (strncmp(line, ".align ",7) == 0 && inProc != 0) {
      line += 7;
      while (*line == ' ') line++;
      if (strncmp(line,"word",4) == 0) {
        offset = (offset + 1) & 0xfffe;
        }
      else if (strncmp(line,"dword",5) == 0) {
        offset = (offset + 3) & 0xfffc;
        }
      else if (strncmp(line,"qword",5) == 0) {
        offset = (offset + 7) & 0xfff8;
        }
      else if (strncmp(line,"para",4) == 0) {
        offset = (offset + 15) & 0xfff0;
        }
      else if (strncmp(line,"32",2) == 0) {
        offset = (offset + 31) & 0xffe0;
        }
      else if (strncmp(line,"64",2) == 0) {
        offset = (offset + 63) & 0xffc0;
        }
      else if (strncmp(line,"128",3) == 0) {
        offset = (offset + 127) & 0xff80;
        }
      else if (strncmp(line,"page",4) == 0) {
        offset = (offset + 255) & 0xff00;
        }
      }
    else if (strncmp(line,".library ",9) == 0) {
      line += 9;
      while (*line == ' ') line++;
      pos = 0;
      while (*line != 0 && *line > ' ') token[pos++] = *line++;
      token[pos] = 0;
      addLibrary(token);
      }
    else if (strncmp(line,".requires ",10) == 0 && loadModule != 0) {
      line += 10;
      while (*line == ' ') line++;
      pos = 0;
      while (*line != 0 && *line > ' ') token[pos++] = *line++;
      token[pos] = 0;
      flag = 0;
      for (i=0; i<numRequires; i++)
        if (strcmp(token, requires[i]) == 0) flag = -1;
      for (i=0; i<numSymbols; i++)
        if (strcmp(token, symbols[i]) == 0) {
          flag = -1;
          }
      if (flag == 0) {
        numRequires++;
        if (numRequires == 1) {
           requires = (char**)malloc(sizeof(char*));
           requireAdded = (char*)malloc(sizeof(char));
           }
        else {
           requires = (char**)realloc(requires, sizeof(char*) * numRequires);
           requireAdded = (char*)realloc(requireAdded, sizeof(char) * numRequires);
           }
        requires[numRequires-1] = (char*)malloc(strlen(token)+1);
        strcpy(requires[numRequires-1], token);
        requireAdded[numRequires-1] = 'N';
        }
      }
    else if (*line == '>' && loadModule != 0) {
      line++;
      line = getHex(line, &value);
      address += value;
      }
    else if (*line == ':' && loadModule != 0) {
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
    else if (*line == '+' && loadModule != 0) {
      line++;
      line = getHex(line, &addr);
      value = readMem(addr+offset);
      value += offset;
      writeMem(addr+offset, value);
      }
    else if (*line == '^' && loadModule != 0) {
      line++;
      line = getHex(line, &addr);
      value = (memory[addr+offset] << 8) + offset;
      while (*line == ' ') line++;
      getHex(line, &lofs);
      value += lofs;
      memory[addr+offset] = (value >> 8) & 0xff;
      }
    else if (*line == 'v' && loadModule != 0) {
      line++;
      line = getHex(line, &addr);
      value = memory[addr+offset] + offset;
      memory[addr+offset] = value & 0xff;
      }

    else if (*line == '$' && loadModule != 0) {
      line++;
      line = getHex(line, &addr);
      value = memory[addr+offset];
      value -= (addr & 0xff);
      dest = addr+offset+value;
      if ((dest & 0xff00) != ((addr+offset) & 0xff00)) {
        printf("Error: Short branch out of page\n");
        return -1;
        }
      memory[addr+offset] = dest & 0xff;
      }

    else if (*line == '=' && loadModule != 0) {
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
      for (i=0; i<numRequires; i++)
        if (strcmp(token, requires[i]) == 0) {
          requireAdded[i] = 'Y';
          }
      }
    else if (*line == '?' && loadModule != 0) {
      line++;
      pos = 0;
      while (*line != 0 && *line > ' ') token[pos++] = *line++;
      token[pos] = 0;
      while (*line == ' ') line++;
      getHex(line, &value);
      if (inProc) value += offset;
      addReference(token, value, 'W', 0);
      }
    else if (*line == '/' && loadModule != 0) {
      line++;
      pos = 0;
      while (*line != 0 && *line > ' ') token[pos++] = *line++;
      token[pos] = 0;
      while (*line == ' ') line++;
      line = getHex(line, &value);
      if (inProc) value += offset;
      while (*line == ' ') line++;
      getHex(line, &low);
      addReference(token, value, 'H', low & 0xff);
      }
    else if (*line == '\\' && loadModule != 0) {
      line++;
      pos = 0;
      while (*line != 0 && *line > ' ') token[pos++] = *line++;
      token[pos] = 0;
      while (*line == ' ') line++;
      getHex(line, &value);
      if (inProc) value += offset;
      addReference(token, value, 'L', 0);
      }

    else if (*line == '{') {
      line++;
      pos = 0;
      while (*line != 0 && *line > ' ') token[pos++] = *line++;
      token[pos] = 0;
      if (libScan != 0) {
        for (i=0; i<numReferences; i++)
          if (strcmp(references[i], token) == 0) {
            loadModule = -1;
            printf("Linking %s from library\n", token);
            i = numReferences;
            }
        if (loadModule == 0) {
          for (i=0; i<numRequires; i++)
            if (requireAdded[i] == 'N' && strcmp(requires[i], token) == 0) {
              loadModule = -1;
              requireAdded[i] = 'Y';
              printf("Linking %s from library\n", token);
              }
          }
        }
      if (loadModule != 0) {
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
        for (i=0; i<numRequires; i++)
          if (strcmp(token, requires[i]) == 0) {
            requireAdded[i] = 'Y';
            }
        }
      }

    else if (*line == '}') {
      inProc = 0;
      offset = 0;
      if (libScan != 0) loadModule = 0;
      }
    }
  fclose(file);
  return 0;
  }
