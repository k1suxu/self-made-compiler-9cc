#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

int use_label() {
  return label_count++;
}

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("代入演算の左辺が変数ではありません");
  
  // raxにrbp-offsetを代入
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  switch (node->kind) {
    case ND_NUM: {
      printf("  push %d\n", node->val);
      return;
    }

    case ND_LVAR: {
      gen_lval(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n"); // raxがさすアドレス(rbp-offset)の中身をraxに代入(これが変数アドレスになる)
      printf("  push rax\n");
      return;
    }

    case ND_ASSIGN: {
      gen_lval(node->lhs);
      gen(node->rhs);

      printf("  pop rdi\n"); // 右辺値をrdiへ取り出す
      printf("  pop rax\n"); // 左辺値(変数のメモリアドレス)を取り出す
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
    }

    case ND_RETURN: {
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
    }

    case ND_IF: {
      if (node->els) { // if (A) B else C 構文
        int cur_label_num = use_label();
        gen(node->cond);                               // 条件式Aの評価
        printf("  pop rax\n");                         // 条件式の評価結果の取り出し
        printf("  cmp rax, 0\n");                      // 0だったらfalse, そうでないならtrue
        printf("  je .Lelse%d\n", cur_label_num);      // false(=0)なら飛ぶ(jump if equal *0)
        gen(node->body);                               // Bを生成
        printf("  jmp .Lend%d\n", cur_label_num);      // Bが終わったらelseを飛ばして次に行く
        printf(".Lelse%d:\n", cur_label_num);          
        gen(node->els);                                // Cを生成
        printf(".Lend%d:\n", cur_label_num);
      } else { // if (A) B 構文
        int cur_label_num = use_label();
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend%d\n", cur_label_num);
        gen(node->body);
        printf(".Lend%d:\n", cur_label_num);
      }
      return;
    }

    case ND_WHILE: { // while (A) B
      int cur_label_num = use_label();
      printf(".Lbegin%d:\n", cur_label_num);
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", cur_label_num);
      gen(node->body);
      printf("  jmp .Lbegin%d\n", cur_label_num);
      printf(".Lend%d:\n", cur_label_num);
      return;
    }

    case ND_FOR: { // for(A; B; C) D
      int cur_label_num = use_label();
      gen(node->init);
      printf(".Lbegin%d:\n", cur_label_num);
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", cur_label_num);
      gen(node->body);
      gen(node->inc);
      printf("  jmp .Lbegin%d\n", cur_label_num);
      printf(".Lend%d:\n", cur_label_num);
      return;
    }
  }


  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  // 二項演算系
  switch (node->kind) {
    case ND_ADD: {
      printf("  add rax, rdi\n");
      break;
    }

    case ND_SUB: {
      printf("  sub rax, rdi\n");
      break;
    }

    case ND_MUL: {
      printf("  imul rax, rdi\n");
      break;
    }

    case ND_DIV: {
      printf("  cqo\n"); // raxを128bit整数として rdx+rax(連接)に保存
      printf("  idiv rdi\n"); // 符号蟻除算においてrdx+rax(連接)をrdiで割って raxに商を、rdxに余りをセットする
      break;
    }

    case ND_EQ: {
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    }

    case ND_NE: {
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    }

    case ND_LT: {
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    }

    case ND_LE: {
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
    }
  }

  printf("  push rax\n"); // 最終演算結果をスタックにpush
}
