#include <stdio.h>
#include "9cc.h"

// グローバル変数の初期化(定義)
Token *token;
char *user_input;
LVar *locals;
int label_count = 1;

// セミコロン区切りのコード
NodeQueue *codes;

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
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // プロローグ
  // 変数26個分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n"); // 26 * 8 = 208

  // 先頭の式から順にコード生成
  while (!nodeQueIsEmpty(codes)) {
    gen(nodeQueTop(codes));
    nodeQuePop(codes);

    printf("  pop rax\n");
  }

  // エピローグ
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n"); // 最後の式の結果がRAXに残っているのでそれが返り値になる
  return 0;
}
