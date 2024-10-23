#include "header.h"

void addReference(char* name, word value, char typ, byte low) {
printf("Adding reference: %s [%c]\n",name,typ);
  numReferences++;
  if (numReferences == 1) {
    references = (char**)malloc(sizeof(char*));
    addresses = (word*)malloc(sizeof(word));
    types = (char*)malloc(sizeof(char));
    lows = (byte*)malloc(sizeof(byte));
    }
  else {
    references = (char**)realloc(references,sizeof(char*)*numReferences);
    addresses = (word*)realloc(addresses,sizeof(word)*numReferences);
    types = (char*)realloc(types,sizeof(char)*numReferences);
    lows = (byte*)realloc(lows,sizeof(byte)*numReferences);
    }
  references[numReferences-1] = (char*)malloc(strlen(name) + 1);
  strcpy(references[numReferences-1], name);
  addresses[numReferences-1] = value;
  lows[numReferences-1] = low;
  types[numReferences-1] = typ;
  }
