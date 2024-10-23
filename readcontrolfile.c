#include "header.h"

void readControlFile(char* filename) {
  FILE* file;
  char  line[1024];
  char *pchar;
  file = fopen(filename, "r");
  if (file == NULL) {
    printf("Could not open %s\n",filename);
    exit(1);
    }
  while ((pchar = fgets(line, 1023, file)) != NULL) {
    while (strlen(line) > 0 && line[strlen(line)-1] <= ' ')
      line[strlen(line)-1] = 0;
    if (strncasecmp(line,"mode ",5) == 0) {
      pchar = line+5;
      while (*pchar == ' ') pchar++;
      if (strcasecmp(pchar, "binary") == 0) outMode = BM_BINARY;
      if (strcasecmp(pchar, "cmd") == 0) outMode = BM_CMD;
      if (strcasecmp(pchar, "elfos") == 0) outMode = BM_ELFOS;
      if (strcasecmp(pchar, "intel") == 0) outMode = BM_INTEL;
      if (strcasecmp(pchar, "rcs") == 0) outMode = BM_RCS;
      if (strcasecmp(pchar, "big") == 0) addressMode = 'B';
      if (strcasecmp(pchar, "little") == 0) addressMode = 'L';
      }
    if (strncasecmp(line,"output ",7) == 0) {
      pchar = line + 7;
      while (*pchar == ' ') pchar++;
        strcpy(outName, pchar);
      }
    if (strncasecmp(line,"add ",4) == 0) {
      pchar = line + 4;
      addObject(pchar);
      }
    if (strncasecmp(line,"library ",8) == 0) {
      pchar = line + 8;
      addLibrary(pchar);
      }
    }
  }
