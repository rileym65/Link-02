#include "header.h"

void addLibrary(char* name) {
  numLibraries++;
  if (numLibraries == 1)
    libraries = (char**)malloc(sizeof(char*));
  else
    libraries = (char**)realloc(libraries,sizeof(char*)*numLibraries);
  libraries[numLibraries-1] = (char*)malloc(strlen(name)+1);
  strcpy(libraries[numLibraries-1],name);
  }
