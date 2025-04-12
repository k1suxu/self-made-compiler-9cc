#include <stdio.h>
#include "9cc.h"

// グローバル変数の初期化(定義)
Token *token;
char *user_input;
LVar *locals;
int label_count = 1;

// セミコロン区切りのコード
Vector *codes;

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
  printLabel("main:");

  // プロローグ
  // ローカル変数26個分の領域を確保する
  printAssembly("push rbp");
  printAssembly("mov rbp, rsp");
  printAssembly("sub rsp, 208"); // 26 * 8 = 208

  // 先頭の式から順にコード生成
  for (VectorDatum *code = codes->front; code; code = code->next) {
    gen(code->cur);
    printAssembly("pop rax");
  }

  // エピローグ
  printAssembly("mov rsp, rbp");
  printAssembly("pop rbp");
  printAssembly("ret"); // 最後の式の結果がRAXに残っているのでそれが返り値になる
  return 0;
}
