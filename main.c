#define MAIN

#include "header.h"

int main(int argc, char **argv) {
  int   i;
  char *pchar;
  FILE *symFile;
  char buffer[256];
  printf("Link/02 v%s\n",VERSION);
  printf("By Michael H. Riley\n\n");
  lowest = 0xffff;
  highest = 0x0000;
  numObjects = 0;
  startAddress = 0xffff;
  showSymbols = 0;
  numSymbols = 0;
  numReferences = 0;
  numLibraries = 0;
  numRequires = 0;
  addressMode = 'L';
  createSymbolFile = 0;
  numLibPath = 0;
  numIncPath = 0;
  strcpy(outName,"");
  strcpy(symName,"");
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
    else if (strcmp(argv[i], "-S") == 0) {
      createSymbolFile = -1;
      i++;
      strcpy(symName, argv[i]);
      }
    else if (strcmp(argv[i], "-I") == 0) {
      i++;
      numIncPath++;
      if (numIncPath == 1)
        incPath = (char**)malloc(sizeof(char*));
      else
        incPath = (char**)realloc(incPath, sizeof(char*) * numIncPath);
      incPath[numIncPath-1] = (char*)malloc(strlen(argv[i]) + 1);
      strcpy(incPath[numIncPath-1], argv[i]);
      }
    else if (strcmp(argv[i], "-L") == 0) {
      i++;
      numLibPath++;
      if (numLibPath == 1)
        libPath = (char**)malloc(sizeof(char*));
      else
        libPath = (char**)realloc(libPath, sizeof(char*) * numLibPath);
      libPath[numLibPath-1] = (char*)malloc(strlen(argv[i]) + 1);
      strcpy(libPath[numLibPath-1], argv[i]);
      }
    else if (strcmp(argv[i], "-o") == 0) {
      i++;
      strcpy(outName, argv[i]);
      }
    else if (argv[i][0] == '@') {
      readControlFile(argv[i]+1);
      }
    else if (strcmp(argv[i], "-l") == 0) {
      i++;
      addLibrary(argv[i]);
      }
    else {
      addObject(argv[i]);
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
  resolved = 0;
  libScan = 0;
  loadModule = -1;
  for (i=0; i<numObjects; i++)
    if (loadFile(objects[i]) < 0) {
      printf("Errors: aborting link\n");
      exit(1);
      }
  doLink();
  resolved = 1;
  while (numReferences > 0 && resolved != 0) {
    libScan = -1;
    for (i=0; i<numLibraries; i++) {
      loadModule = 0;
      if (loadFile(libraries[i]) < 0) {
        printf("Errors: aborting link\n");
        exit(1);
        }
      }
    doLink();
    }
  if (numReferences > 0) {
    for (i=0; i<numReferences; i++) {
      printf("Error: Symbol %s not found\n",references[i]);
      }
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
  printf("Public symbols : %d\n",numSymbols);
  if (startAddress != 0xffff)
    printf("Start address  : %04x\n",startAddress);
  if (showSymbols) {
    sortSymbols();
    for (i=0; i<numSymbols; i++)
      printf("%-20s:%-20s %04x\n",symbols[i], symbolModules[i],  values[i]);
    }
  if (createSymbolFile) {
    sortSymbols();
    symFile = fopen(symName, "w");
    if (symFile == NULL) {
      printf("Could not create symbol file\n");
      }
    else {
      for (i=0; i<numSymbols; i++) {
        fprintf(symFile,"%-20s %04x\n",symbols[i], values[i]);
        }
      }
    }

  printf("\n");
  return 0;
  }

