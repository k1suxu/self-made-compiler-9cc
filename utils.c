#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  char *p = loc;
  while ((user_input <= p) && (*p != '\n')) p--;
  p++;

  for (char *cur = p; cur && (*cur != '\n'); cur++) {
    fprintf(stderr, "%c", *cur);
  }
  fprintf(stderr, "\n");

  int pos = loc-p;
  fprintf(stderr, "%*s", pos, " "); /* pos個の空白を出力 */
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void debug(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
}

bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

int round_up(int x, int align) {
  return (x + align - 1) / align * align;
}

void debug_functions() {
  debug("*****関数のデバッグ*****");
  debug("関数の個数: %d", functions->size);
  for (ListDatum *func = functions->front; func; func = func->next) {
    debug("関数名: %s", ((Function *)func->cur)->funcName);
    debug("関数の引数の個数: %d", ((Function *)func->cur)->args->size);
    debug("関数のローカル変数の個数: %d", ((Function *)func->cur)->locals->size);
    debug("関数のスタックサイズ: %d", ((Function *)func->cur)->stackSize);
    debug("プロトタイプかどうか: %d", ((Function *)func->cur)->isPrototype);
  }
}

void debug_codes() {
  debug("*****コードのデバッグ*****");
  debug("コードの個数: %d", codes->size);
  for (ListDatum *func = codes->front; func; func = func->next) {
    debug("関数名: %s", ((Function *)func->cur)->funcName);
    debug("関数の引数の個数: %d", ((Function *)func->cur)->args->size);
    debug("関数のローカル変数の個数: %d", ((Function *)func->cur)->locals->size);
    debug("関数のスタックサイズ: %d", ((Function *)func->cur)->stackSize);
    debug("プロトタイプかどうか: %d", ((Function *)func->cur)->isPrototype);
  }
}
