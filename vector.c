#include <stdlib.h>
#include "9cc.h"

Vector *vecNew() {
  Vector *q = calloc(1, sizeof(Vector));
  q->front = NULL;
  q->back = NULL;
}

bool vecIsEmpty(Vector *q) {
  return q->front == NULL;
}

void vecPush(Vector *q, void *cur) {
  VectorDatum *newDatum = calloc(1, sizeof(VectorDatum));
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

void *vecTop(Vector *q) {
  if (vecIsEmpty(q)) {
    error("Vector is Empty (top) !!\n");
  }

  return q->front->cur;
}

void *vecPop(Vector *q) {
  if (vecIsEmpty(q)) {
    error("Vector is Empty (pop) !!\n");
  }

  q->front = q->front->next;
  if (q->front == NULL) {
    q->back = NULL;
  }
}