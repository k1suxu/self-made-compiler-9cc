
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "9cc.h"

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す
bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

bool consume_kind(TokenKind tk) {
  if (token->kind != tk)
    return false;
  token = token->next;
  return true;
}

Token *consume_ident() {
  if (token->kind != TK_IDENT) return NULL;
  Token *ret = token;
  token = token->next;
  return ret;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める
// それ以外の場合にはエラーを報告する
void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    error_at(token->str, "'%s'ではありません", op);
  token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す
// それ以外の場合にはエラーを報告する
int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// なんでも
Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}
// 単項演算
Node *new_node_unary(NodeKind kind, Node *lhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  return node;
}
// 二項演算
Node *new_node_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}
// 数値
Node *new_node_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

// ローカル変数名検索(見つからなかった場合はNULL)
LVar *find_lvar(Token *tok) {
  if (tok->kind != TK_IDENT) {
    error_at(tok->str, "`find_lvar` 関数が呼ばれましたが、引数が TK_IDENT ではありません");
  }
  Function *cur_func = codes->back->cur;
  for (ListDatum *ld = cur_func->locals->front; ld; ld = ld->next) {
    LVar *lvar = ld->cur;
    if (lvar->len == tok->len && !memcmp(tok->str, lvar->name, lvar->len))
      return lvar;
  }
  return NULL;
}

Type *expect_type() {
  if (!consume_kind(TK_INT)) {
    error_at(token->str, "関数の返り値または引数および変数の宣言時に、型は省略できません");
  }
  Type *retType = calloc(1, sizeof(Type));
  retType->ty = INT;
  while (consume("*")) {
    Type *ptr = calloc(1, sizeof(Type));
    ptr->ty = PTR;
    ptr->ptr_to = retType;
    retType = ptr;
  }
  return retType;
}

Function *find_function(Token *tok) {
  if (tok->kind != TK_IDENT) {
    error_at(tok->str, "`find_function` 関数が呼ばれましたが、引数が TK_IDENT ではありません");
  }
  for (ListDatum *ld = functions->front; ld; ld = ld->next) {
    Function *func = ld->cur;
    if (func->funcNameLen == tok->len && !memcmp(tok->str, func->funcName, tok->len)) {
      return func;
    }
  }
  return NULL;
}

// program = func*;
void program() {
  codes = listNew();
  functions = listNew();
  while (!at_eof())
    func();
}

// func = func_name "(" (arg ("," arg)*)? ")" "{" stmt* "}"
void func() {
  Function *cur = calloc(1, sizeof(Function));
  cur->retType = expect_type();

  Token *tok = consume_ident();
  if (!tok) {
    error_at(token->str, "パース時のトップレベルは関数宣言でないといけません");
  }
  // if (find_function(tok)) {
  //   // プロトタイプ宣言がある都合上、一旦コメントアウト
  //   error_at(tok->str, "関数名が重複しています");
  // }

  listPush(codes, cur);
  
  expect("(");
  cur->funcName = calloc(tok->len, sizeof(char));
  strncpy(cur->funcName, tok->str, tok->len);
  cur->funcNameLen = tok->len;
  cur->locals = listNew();
  cur->args = listNew();
  cur->stackSize = 0;

  if (!consume(")")) {
    while (1) {
      Type *lvar_type = expect_type();

      Token *arg_tok = consume_ident();
      if (!arg_tok) {
        error_at(token->str, "引数が変数名ではありません");
      }
      LVar *found = find_lvar(arg_tok);
      if (found) {
        error_at(arg_tok->str, "引数名が重複しています");
      }

      found = calloc(1, sizeof(LVar));
      found->name = arg_tok->str;
      found->len = arg_tok->len;
      found->offset = (cur->stackSize + 8); // 8バイト固定
      found->type = lvar_type;

      cur->stackSize += 8;
      (cur->argc)++;
      listPush(cur->args, found);
      listPush(cur->locals, found);

      if (consume(")"))
        break;
      expect(",");
    }
  }

  // 返り値の型および引数の型や個数を保存
  // debug("関数名: %.*s, 関数名の長さ: %d, 引数の個数: %d\n", cur->funcNameLen, cur->funcName, cur->funcNameLen, cur->argc);
  listPush(functions, cur);

  if (consume(";")) {
    listPop(codes);
    return;
  }

  // stmtでやると{}に囲まれない一行処理とかがコンパイルを通ってしまう気がしている
  expect("{");
  cur->roots = listNew();
  while (!consume("}")) {
    Node *statement = stmt();
    listPush(cur->roots, statement);
  }

  cur->stackSize = round_up(cur->stackSize, 16);
}

/*
stmt =    expr ";"
        | "int" [VAR_NAME] ";"
        | "{" stmt* "}"
        | "return" expr ";"
        | "if" "(" expr ")" stmt ("else" stmt)?
        | "while" "(" expr ")" stmt
        | "for" "(" expr? ";" expr? ";" expr? ")" stmt
 */
Node *stmt() {
  if (token->kind == TK_INT) {
    Type *lvar_type = expect_type();

    Token *tok = consume_ident();
    if (!tok) {
      error_at(token->str, "int の後は変数名が来るべきですが、%.*sが書かれています", tok->len, tok->str);
    }
    LVar *lvar = find_lvar(tok);

    if (lvar) {
      error_at(tok->str, "Redeclaration of %.*s", tok->len, tok->str);
    }

    lvar = calloc(1, sizeof(LVar));
    lvar->name = tok->str;
    lvar->len = tok->len;
    Function *code_back = codes->back->cur;
    lvar->offset = (code_back->stackSize + 8); // 8バイト固定
    lvar->type = lvar_type;

    code_back->stackSize += 8;
    listPush(code_back->locals, lvar);

    Node *node = new_node(ND_VAR_DEF);
    node->offset = lvar->offset;
    node->varName = calloc(lvar->len, sizeof(char));
    strncpy(node->varName, tok->str, tok->len);

    expect(";");
    
    return node;
  }

  if (consume("{")) {
    Node *node = new_node(ND_BLOCK);
    node->multiStmt = listNew();
    while (!consume("}"))
      listPush(node->multiStmt, stmt());
    return node;
  }

  if (consume_kind(TK_RETURN)) {
    Node *node = new_node(ND_RETURN);
    node->lhs = expr();
    expect(";");
    return node;
  }

  if (consume_kind(TK_IF)) {
    expect("(");
    Node *node = new_node(ND_IF);
    node->cond = expr();
    expect(")");
    node->body = stmt();
    if (consume_kind(TK_ELSE)) {
      node->els = stmt();
    }
    return node;
  }

  if (consume_kind(TK_WHILE)) {
    expect("(");
    Node *node = new_node(ND_WHILE);
    node->cond = expr();
    expect(")");
    node->body = stmt();
    return node;
  }

  if (consume_kind(TK_FOR)) {
    expect("(");
    Node *node = new_node(ND_FOR);
    node->init = expr(); 
    expect(";");
    node->cond = expr();
    expect(";");
    node->inc = expr();
    expect(")");
    node->body = stmt();
    return node;
  }

  Node *node = expr();
  expect(";");
  return node;
}

// expr = assign
Node *expr() {
  return assign();
}

// assign = equality ("=" assign)?
Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node_binary(ND_ASSIGN, node, assign());
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("==")) 
      node = new_node_binary(ND_EQ, node, relational());
    else if (consume("!=")) 
      node = new_node_binary(ND_NE, node, relational());
    else
      return node;
  }
}
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<")) 
      node = new_node_binary(ND_LT, node, add());
    else if (consume("<=")) 
      node = new_node_binary(ND_LE, node, add());
    else if (consume(">"))
      node = new_node_binary(ND_LT, add(), node);
    else if (consume(">="))
      node = new_node_binary(ND_LE, add(), node);
    else
      return node;
  }
}
// add = mul ("+" mul | "-" mul)
Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node_binary(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node_binary(ND_SUB, node, mul());
    else
      return node;
  }
}
// mul = unary ("*" unary | "/" unary)*
Node *mul() {
  Node *node = unary();

  for (;;) {
    // 左側に深くなっていく
    if (consume("*"))
      node = new_node_binary(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node_binary(ND_DIV, node, unary());
    else
      return node;
  }
}
// unary = ("+" | "-")? primary | ("&" | "*") unary
Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node_binary(ND_SUB, new_node_num(0), primary());
  if (consume("&"))
    return new_node_unary(ND_ADDR, unary());
  if (consume("*"))
    return new_node_unary(ND_DEREF, unary());
  return primary();
}
// primary =  num
//         | ident ("(" (assign ("," assign)*)? ")")?
//         | "(" expr ")"
Node *primary() {
  // 次のトークンが "(" なら、 "(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    if (consume("(")) { // 関数呼び出し
      Node *node = new_node(ND_FUNC_CALL);
      node->funcName = calloc(tok->len, sizeof(char));
      strncpy(node->funcName, tok->str, tok->len);
      node->args = listNew();
      node->argc = 0;
      
      if (!consume(")")) {
        while (1) {
          listPush(node->args, assign());
          node->argc++;
          if (consume(")"))
            break;
          expect(",");
        }
      }

      Function *func = find_function(tok);
      
      if (!func) {
        error_at(tok->str, "Undefined Error (function): %.*s", tok->len, tok->str);
      }

      if (func->argc != node->argc) {
        error_at(tok->str, "Argument Number is not Compatible: %.*s", tok->len, tok->str);
      }

      return node;
    }

    Node *node = new_node(ND_LVAR);
    
    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
    } else {
      error_at(tok->str, "Undefined Error (variable): %.*s", tok->len, tok->str);
    }

    return node;
  }
  // 数値のはず
  return new_node_num(expect_number());
}
