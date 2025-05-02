#include <stdlib.h>
#include "9cc.h"

List *listNew() {
  List *q = calloc(1, sizeof(List));
  q->front = NULL;
  q->back = NULL;
  q->size = 0;
  return q;
}

bool listIsEmpty(List *q) {
  return q->front == NULL;
}

void listPush(List *q, void *cur) {
  ListDatum *newDatum = calloc(1, sizeof(ListDatum));
  newDatum->cur = cur;
  newDatum->next = NULL;
  newDatum->prev = q->back;  // 新しい要素のprevを現在のbackに設定

  if (listIsEmpty(q)) {
    q->front = newDatum;
    q->back = newDatum;
    q->size = 1;
  } else {
    q->back->next = newDatum;  // 現在のbackのnextを新しい要素に設定
    q->back = newDatum;
    ++(q->size);
  }
}

void *listTop(List *q) {
  if (listIsEmpty(q)) {
    error("List is Empty (top) !!\n");
  }

  return q->front->cur;
}

int listSize(List *q) {
  return q->size;
}

void list_erase(List *from, void *item) {
  for (ListDatum *cur = from->front; cur != NULL; cur = cur->next) {
    if (cur->cur == item) {
      if (cur == from->front) {
        from->front = cur->next;
        if (from->front != NULL) {
          from->front->prev = NULL;  // 新しいfrontのprevをNULLに設定
        } else {
          from->back = NULL;  // リストが空になった場合
        }
      } else if (cur == from->back) {
        from->back = cur->prev;
        from->back->next = NULL;  // 新しいbackのnextをNULLに設定
      } else {
        cur->prev->next = cur->next;  // 前の要素のnextを更新
        cur->next->prev = cur->prev;  // 次の要素のprevを更新
      }
      --(from->size);
      return;
    }
  }
}
