#include <stdio.h>
#include "9cc.h"

// グローバル変数の初期化(定義)
Token *token;
char *user_input;
LVar *locals;
int label_count = 1;
int rsp = 0;

List *codes;

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  // トークナイズしてパース (結果はグローバル変数：codeに保存)
  user_input = argv[1];
  token = tokenize(user_input);
  program();

  // アセンブリの前半部分を出力
  printLabel(".intel_syntax noprefix");
  printLabel(".globl main");

  // 先頭の式から順にコード生成
  for (ListDatum *code = codes->front; code; code = code->next) {
    gen_func(code->cur);
  }
  return 0;
}
