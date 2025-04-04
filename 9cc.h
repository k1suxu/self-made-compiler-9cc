#include <stdbool.h>

/*
 * Tokenize関数以外はすべてtokenを用いて動く
 * token位置のずらしは、expect, consumeで行い、不確定の場合はconsume, 確定の時(そうでない場合にエラーを吐きたいとき)はexpect***を使う
 */

/* グローバル変数 (ユーザーからのインプットプログラム) */
extern char *user_input;

/* エラー関連 */
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

/* Tokenizer関連 */
/* トークンの種類 */
typedef enum {
  TK_RESERVED, /* 記号 */
  TK_NUM,      /* 整数トークン */
  TK_EOF,      /* 入力の終わりを表すトークン */
} TokenKind;
/* トークン型 */
typedef struct Token Token;
struct Token {
  TokenKind kind; /* トークンの型 */
  Token *next;    /* 次の入力トークン */
  int val;        /* kindがTK_NUMの場合、その数値 */
  char *str;      /* トークン文字列 (入力プログラムにおけるトークンの開始位置でもある) */
  int len;        /* トークン長 */
};
extern Token *token; /* ユーザーインプットのトークン列を表すグローバル変数 */
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);

/* トークンの読み進め関連 */
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();

/* 便利関数 */
bool startswith(char *p, char *q);

/* 数式解釈 */
typedef enum {
  ND_ADD, /* + */
  ND_SUB, /* - */
  ND_MUL, /* * */
  ND_DIV, /* / */
  ND_EQ,  /* == */
  ND_NE,  /* != */
  ND_LT,  /* < (GT, GEは左右交換によってLT, LEに帰着する) */
  ND_LE,  /* <= */
  ND_NUM, /* Integer */
} NodeKind;
typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
};
Node *new_node(NodeKind kind);
Node *new_node_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
/* 数式解釈(抽象構文木生成過程) */
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

/* アセンブリ生成 */
void gen(Node *node);
