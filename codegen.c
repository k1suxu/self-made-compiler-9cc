#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

char *argRegisters[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

int use_label() {
  return label_count++;
}

void printLabel(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  printf("\n");
}
void printAssembly(char *fmt, ...) {
  // rspの値はpush, popの他、sub rsp, ***等でも変化する可能性があるが、現状、***の方も16-alignされているので、push, popのみを評価する形にしても問題ない
  if (startswith(fmt, "push")) rsp += 1;
  if (startswith(fmt, "pop")) rsp -= 1;
  va_list ap;
  va_start(ap, fmt);
  printf("\t");
  vprintf(fmt, ap);
  printf("\n");
}

// メモリスタックの位置を示す値をraxに入れてpush
void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("代入演算の左辺またはアドレス演算子の中身が変数ではありません");
  
  // raxにrbp-offsetを代入
  printAssembly("mov rax, rbp");
  printAssembly("sub rax, %d", node->offset);
  printAssembly("push rax");
}

void gen(Node *node) {
  switch (node->kind) {
    case ND_BLOCK: {
      for (ListDatum *q = node->multiStmt->front; q; q = q->next) {
        gen(q->cur);
        printAssembly("pop rax"); // (最終演算結果は要らない)
      }
      return;
    }

    case ND_FUNC_CALL: {
      int cnt = 0;
      for (ListDatum *arg = node->args->front; arg; arg = arg->next) {
        if (cnt == 6) {
          error("7つ以上の引数は指定できません");
        }

        gen(arg->cur);
        printAssembly("pop rax"); // 演算結果を取ってくる
        printAssembly("mov %s, rax", argRegisters[cnt]); // 引数をレジスタに入力
        cnt++;
      }

      // [DONE]: rspが16の倍数になるように調整 (現状はstack_size=0よりずれる可能性がないが、将来的にはずれる可能性がある)
      if (rsp % 2 == 1) 
        printAssembly("sub rsp, 8");

      printAssembly("call %s", node->funcName);
      
      if (rsp % 2 == 1)
        printAssembly("add rsp, 8");

      printAssembly("push rax"); // 関数呼び出しの結果をpop
      return;
    }

    case ND_NUM: {
      printAssembly("push %d", node->val);
      return;
    }

    case ND_LVAR: {
      gen_lval(node);
      printAssembly("pop rax");
      printAssembly("mov rax, [rax]"); // raxがさすアドレス(rbp-offset)の中身をraxに代入(これが変数アドレスになる)
      printAssembly("push rax");
      return;
    }

    case ND_ASSIGN: {
      gen_lval(node->lhs);
      gen(node->rhs);

      printAssembly("pop rdi"); // 右辺値をrdiへ取り出す
      printAssembly("pop rax"); // 左辺値(変数のメモリアドレス)を取り出す
      printAssembly("mov [rax], rdi");
      printAssembly("push rdi");
      return;
    }

    case ND_RETURN: {
      gen(node->lhs);
      printAssembly("pop rax");
      printAssembly("mov rsp, rbp");
      printAssembly("pop rbp");
      printAssembly("ret");
      return;
    }

    case ND_IF: {
      if (node->els) { // if (A) B else C 構文
        int cur_label_num = use_label();
        gen(node->cond);                               // 条件式Aの評価
        printAssembly("pop rax");                         // 条件式の評価結果の取り出し
        printAssembly("cmp rax, 0");                      // 0だったらfalse, そうでないならtrue
        printAssembly("je .Lelse%d", cur_label_num);      // false(=0)なら飛ぶ(jump if equal *0)
        gen(node->body);                               // Bを生成
        printAssembly("jmp .Lend%d", cur_label_num);      // Bが終わったらelseを飛ばして次に行く
        printLabel(".Lelse%d:", cur_label_num);          
        gen(node->els);                                // Cを生成
        printLabel(".Lend%d:", cur_label_num);
      } else { // if (A) B 構文
        int cur_label_num = use_label();
        gen(node->cond);
        printAssembly("pop rax");
        printAssembly("cmp rax, 0");
        printAssembly("je .Lend%d", cur_label_num);
        gen(node->body);
        printLabel(".Lend%d:", cur_label_num);
      }
      return;
    }

    case ND_WHILE: { // while (A) B
      int cur_label_num = use_label();
      printLabel(".Lbegin%d:", cur_label_num);
      gen(node->cond);
      printAssembly("pop rax");
      printAssembly("cmp rax, 0");
      printAssembly("je .Lend%d", cur_label_num);
      gen(node->body);
      printAssembly("jmp .Lbegin%d", cur_label_num);
      printLabel(".Lend%d:", cur_label_num);
      return;
    }

    case ND_FOR: { // for (A; B; C) D
      int cur_label_num = use_label();
      gen(node->init);
      printLabel(".Lbegin%d:", cur_label_num);
      gen(node->cond);
      printAssembly("pop rax");
      printAssembly("cmp rax, 0");
      printAssembly("je .Lend%d", cur_label_num);
      gen(node->body);
      gen(node->inc);
      printAssembly("jmp .Lbegin%d", cur_label_num);
      printLabel(".Lend%d:", cur_label_num);
      return;
    }
    case ND_ADDR: {
      gen_lval(node->lhs);
      return;
    }
    case ND_DEREF: {
      gen(node->lhs);
      printAssembly("pop rax");
      printAssembly("mov rax, [rax]");
      printAssembly("push rax");
      return;
    }
  }


  // 二項演算系
  gen(node->lhs);
  gen(node->rhs);

  printAssembly("pop rdi");
  printAssembly("pop rax");

  switch (node->kind) {
    case ND_ADD: {
      printAssembly("add rax, rdi");
      break;
    }

    case ND_SUB: {
      printAssembly("sub rax, rdi");
      break;
    }

    case ND_MUL: {
      printAssembly("imul rax, rdi");
      break;
    }

    case ND_DIV: {
      printAssembly("cqo"); // raxを128bit整数として rdx+rax(連接)に保存
      printAssembly("idiv rdi"); // 符号あり除算においてrdx+rax(連接)をrdiで割って raxに商を、rdxに余りをセットする
      break;
    }

    case ND_EQ: {
      printAssembly("cmp rax, rdi");
      printAssembly("sete al");
      printAssembly("movzb rax, al");
      break;
    }

    case ND_NE: {
      printAssembly("cmp rax, rdi");
      printAssembly("setne al");
      printAssembly("movzb rax, al");
      break;
    }

    case ND_LT: {
      printAssembly("cmp rax, rdi");
      printAssembly("setl al");
      printAssembly("movzb rax, al");
      break;
    }

    case ND_LE: {
      printAssembly("cmp rax, rdi");
      printAssembly("setle al");
      printAssembly("movzb rax, al");
      break;
    }
  }

  printAssembly("push rax"); // 最終演算結果をスタックにpush
}

void gen_func(Function *func) {
  if (func->argc > 6) {
    error("7つ以上の引数をもつ関数の定義はサポートされていません");
  }
  
  // プロローグ
  printLabel("%s:", func->funcName);
  printAssembly("push rbp");
  printAssembly("mov rbp, rsp");
  printAssembly("sub rsp, %d", func->stackSize);

  {
    ListDatum *ld = func->locals->front;
    for (int cnt = 0; cnt < func->argc; cnt++) {
      LVar *lvar = ld->cur;
      printAssembly("mov rax, rbp");
      printAssembly("sub rax, %d", lvar->offset);
      printAssembly("mov [rax], %s", argRegisters[cnt]);
      ld = ld->next;
    }
  }

  // 関数ごとにrspの値を修正(rbpからのずれで見る)
  rsp = 0;

  {
    ListDatum *ld = func->roots->front;
    for ( ; ld; ld = ld->next) {
      Node *cur = ld->cur;
      gen(cur);
      printAssembly("pop rax"); // 最終結果をpop (return文が伴ってるならいらないけど既存のテストのパスのために一応残してる)
    }
  }

  // エピローグ (return文を強制しちゃえばもうこれ要らないけど既存のテストのパスのために一応残してる)
  printAssembly("mov rsp, rbp");
  printAssembly("pop rbp");
  printAssembly("ret");
}
