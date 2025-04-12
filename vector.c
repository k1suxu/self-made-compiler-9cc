#include <stdlib.h>
#include "9cc.h"

List *vecNew() {
  List *q = calloc(1, sizeof(List));
  q->front = NULL;
  q->back = NULL;
}

bool vecIsEmpty(List *q) {
  return q->front == NULL;
}

void vecPush(List *q, void *cur) {
  ListDatum *newDatum = calloc(1, sizeof(ListDatum));
  newDatum->cur = cur;
  newDatum->next = NULL;

  if (vecIsEmpty(q)) {
    q->front = newDatum;
    q->back = newDatum;
  } else {
    q->back->next = newDatum;
    q->back = newDatum;
  }
}

void *vecTop(List *q) {
  if (vecIsEmpty(q)) {
    error("List is Empty (top) !!\n");
  }

  return q->front->cur;
}

void *vecPop(List *q) {
  if (vecIsEmpty(q)) {
    error("List is Empty (pop) !!\n");
  }

  q->front = q->front->next;
  if (q->front == NULL) {
    q->back = NULL;
  }
}