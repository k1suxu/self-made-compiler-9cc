#include <stdlib.h>
#include "9cc.h"

NodeQueue *nodeQueNew() {
  NodeQueue *q = calloc(1, sizeof(NodeQueue));
  q->front = NULL;
  q->back = NULL;
}

bool nodeQueIsEmpty(NodeQueue *q) {
  return q->front == NULL;
}

void nodeQuePush(NodeQueue *q, Node *cur) {
  NodeLinkList *newNode = calloc(1, sizeof(NodeLinkList));
  newNode->cur = cur;
  newNode->next = NULL;

  if (nodeQueIsEmpty(q)) {
    q->front = newNode;
    q->back = newNode;
  } else {
    q->back->next = newNode;
    q->back = newNode;
  }
}

Node *nodeQueTop(NodeQueue *q) {
  if (nodeQueIsEmpty(q)) {
    error("Queue is Empty (top) !!\n");
  }

  return q->front->cur;
}

void *nodeQuePop(NodeQueue *q) {
  if (nodeQueIsEmpty(q)) {
    error("Queue is Empty (pop) !!\n");
  }

  q->front = q->front->next;
  if (q->front == NULL) {
    q->back = NULL;
  }
}