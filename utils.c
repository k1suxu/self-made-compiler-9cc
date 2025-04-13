#include "9cc.h"
#include <stdbool.h>
#include <string.h>

bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

int round_up(int x, int align) {
  return (x + align - 1) / align * align;
}
