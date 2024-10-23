#include "header.h"

void addObject(char* name) {
  numObjects++;
  if (numObjects == 1)
    objects = (char**)malloc(sizeof(char*));
  else
    objects = (char**)realloc(objects,sizeof(char*)*numObjects);
  objects[numObjects-1] = (char*)malloc(strlen(name)+1);
  strcpy(objects[numObjects-1],name);
  }
