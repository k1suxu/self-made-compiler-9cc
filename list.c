#include <stdlib.h>
#include "9cc.h"

List *listNew() {
  List *q = calloc(1, sizeof(List));
  q->front = NULL;
  q->back = NULL;
  q->size = 0;
}

bool listIsEmpty(List *q) {
  return q->front == NULL;
}

void listPush(List *q, void *cur) {
  ListDatum *newDatum = calloc(1, sizeof(ListDatum));
  newDatum->cur = cur;
  newDatum->next = NULL;

  if (listIsEmpty(q)) {
    q->front = newDatum;
    q->back = newDatum;
    q->size = 1;
  } else {
    q->back->next = newDatum;
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
        if (from->front == NULL) {
          from->back = NULL;
        }
      } else {
        ListDatum *prev = from->front;
        while (prev->next != cur) {
          prev = prev->next;
        }
        prev->next = cur->next;
        if (prev->next == NULL) {
          from->back = prev;
        }
      }
      --(from->size);
      return;
    }
  }
}
