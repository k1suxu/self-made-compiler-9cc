/*
 * [TODO]: 現状はすべての関数をここに宣言しているが、一つのファイルからしか参照されていないものは省くべき
 */

#include <stdbool.h>

/*
 * Tokenize関数以外はすべてtokenを用いて動く
 * token位置のずらしは、expect, consumeで行い、不確定の場合はconsume, 確定の時(そうでない場合にエラーを吐きたいとき)はexpect***を使う
 */

// グローバル変数 (ユーザーからのインプットプログラム)
extern char *user_input;

// エラー関連
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

// Tokenizer関連
// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子 (a-zの１文字変数名)
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
  TK_RETURN,
  TK_IF,
  TK_ELSE,
  TK_WHILE,
  TK_FOR,
} TokenKind;
// トークン型
typedef struct Token Token;
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列 (入力プログラムにおけるトークンの開始位置でもある)
  int len;        // トークン長
};
extern Token *token; // ユーザーインプットのトークン列を表すグローバル変数
int is_alnumunder(char c);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);

// トークンの読み進め関連
bool consume(char *op);
bool consume_kind(TokenKind tk);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();

// 便利関数
bool startswith(char *p, char *q);

// 抽象構文木生成過程
typedef struct Node Node;
typedef struct NodeLinkList NodeLinkList;
typedef struct NodeQueue NodeQueue;
struct NodeLinkList {
  Node *cur;
  NodeLinkList *next;
};
struct NodeQueue {
  NodeLinkList *front;
  NodeLinkList *back;
};
NodeQueue *nodeQueNew();
bool nodeQueIsEmpty(NodeQueue *q);
void nodeQuePush(NodeQueue *q, Node *cur);
Node *nodeQueTop(NodeQueue *q);
void *nodeQuePop(NodeQueue *q);
extern NodeQueue *codes;

typedef enum {
  ND_ADD,     // +
  ND_SUB,     // -
  ND_MUL,     // *
  ND_DIV,     // /
  ND_ASSIGN,  // =
  ND_EQ,      // ==
  ND_NE,      // !=
  ND_LT,      // < (GT, GEは左右交換によってLT, LEに帰着する)
  ND_LE,      // <=
  ND_NUM,     // Integer
  ND_LVAR,    // Local Variables
  ND_RETURN,
  ND_IF,
  ND_ELSE,
  ND_WHILE,
  ND_FOR,
} NodeKind;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;        // kind=ND_NUMの時のみ使う
  int offset;     // kind=ND_LVALの時のみ使う(変数アクセス用のRSPからのオフセット)

  // "if" ( cond ) then "else" els
  // "for" ( init; cond; inc ) body
  // "while" ( cond ) body
  Node *cond;
  Node *then;
  Node *els;
  Node *init;
  Node *inc;
  Node *body;
};
Node *new_node(NodeKind kind);
Node *new_node_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// アセンブリ生成
void gen_lval(Node *node);
void gen(Node *node);

// ローカル変数の型
typedef struct LVar LVar;
struct LVar {
  LVar *next;   // 次の変数 or NULL (連結リストで変数辞書を表す)
  char *name;   // 変数名
  int len;      // 変数名の長さ
  int offset;   // rbpからのオフセット
};
extern LVar *locals; // ローカル変数辞書を表すグローバル変数

extern int label_count; // labelの末尾につけるXXXXの数字を管理する(今はif, while等すべてに共通して一つの値を使っているが、将来的には各ラベルごとに管理するようにしたい)
