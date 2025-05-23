#include <stdio.h>
#include <string.h>
#include "9cc.h"

// グローバル変数の初期化(定義)
Token *token;
char *user_input;
LVar *locals;
int rsp = 0;

List *codes;
List *functions;

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  // トークナイズしてパース (結果はグローバル変数：codeに保存)
  
  user_input = argv[1];
  user_input = strcat(argv[1], "\n");
  token = tokenize(user_input);
  program();

  // debug_codes();

  // アセンブリの前半部分を出力
  printLabel(".intel_syntax noprefix");
  printLabel(".globl main");

  // 先頭の式から順にコード生成
  for (ListDatum *code = codes->front; code; code = code->next) {
    gen_func(code->cur);
  }

  // debug_functions();

  return 0;
}
