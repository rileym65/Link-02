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
