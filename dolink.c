#include "header.h"

void doLink() {
  int i;
  int j;
  int s;
  int errors;
  word address;
  word v;
  errors=0;
  resolved = 0;
  i = 0;
printf("***** DoLink ********\n");
printf("unresolved symbols after link:\n");
for (i=0;i<numReferences;i++) printf("  %s\n",references[i]);

  i = 0;
//  for (i=0; i<numReferences; i++) {
  while (i < numReferences) {
    s = findSymbol(references[i]);
    if (s < 0) {
      i++;
      }
    else {
      resolved++;
printf("-->Resolved %s\n",references[i]);
      address = addresses[i];
      if (types[i] == 'W') {
        v = readMem(address) + values[s];
        writeMem(address, v);
        }
      if (types[i] == 'H') {
        v = ((memory[address] << 8) + values[s] + lows[i]) >> 8;
        memory[address] = v & 0xff;
        }
      if (types[i] == 'L') {
        v = memory[address] + values[s];
        memory[address] = v & 0xff;
        }
      free(references[i]);
      for (j=i; j<numReferences-1; j++) {
        references[j] = references[j+1];
        addresses[j] = addresses[j+1];
        types[j] = types[j+1];
        lows[j] = lows[j+1];
        }
      numReferences--;
      if (numReferences > 0) {
        references = (char**)realloc(references,sizeof(char*)*numReferences);
        addresses = (word*)realloc(addresses,sizeof(word)*numReferences);
        types = (char*)realloc(types,sizeof(char)*numReferences);
        lows = (byte*)realloc(lows,sizeof(byte)*numReferences);
        }
      else {
        free(references);
        free(addresses);
        free(types);
        free(lows);
        }
      }
    }
printf("unresolved symbols after link:\n");
for (i=0;i<numReferences;i++) printf("  %s\n",references[i]);
  }
