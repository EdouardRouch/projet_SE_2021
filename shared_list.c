#include "shared_list.h"

#define LIST_SIZE 20

typedef struct client_resources client_resources;

struct client_resources {
  int pipe_request;
  int pipe_response;
};

typedef struct cell cell;

struct cell {
  client_resources *ptr;
  cell *next;
};

struct lcell {
  cell *head;
  cell *tail;
  sem_t full;
  sem_t empty;
  sem_t mutex;
};

lcell *create_empty_list(void) {
  lcell *p = malloc(sizeof *p);
  if (p == NULL) {
    return NULL;
  }
  p->head = NULL;
  p->tail = NULL;
  if (sem_init(&p->mutex, 1, 1) != 0) {
    free(q);
    return NULL;
  }
  if (sem_init(&p->empty, 1, LIST_SIZE) != 0) {
    sem_destroy(&p->mutex);
    return NULL;
  }

  if (sem_init(&p->full, 1, 0) != 0) {
    sem_destroy(&p->mutex);
    sem_destroy(&p->empty);
    return NULL;
  }
  return p;
}

bool lcell_is_empty(const lcell *p) {
  return p->head == NULL && p->tail == NULL;
}

int lcell_insert_head(lcell *p) {
  if (sem_wait(&p->empty) != 0) {
    return -1;
  }
  if (sem_wait(&p->mutex) != 0) {
    return -1;
  }

  cell *c = malloc(sizeof *c);
  if (c == NULL) {
    return -1;
  }
  client_resources *res = malloc(sizeof *res);
  if (res == NULL) {
    free(c);
    return -1;
  }
  c->next = p->head;
  p->head = c;
  if (lcell_is_empty(p)) {
    p->tail = c;
  }
  c->ptr = res;
  if (sem_post(&p->mutex) != 0) {
    return -1;
  }
  if (sem_post(&p->full) != 0) {
    return -1;
  }
  return 0;
}

int lcell_insert_tail(lcell *p) {
  if (sem_wait(&p->empty) != 0) {
    return -1;
  }
  if (sem_wait(&p->mutex) != 0) {
    return -1;
  }

  cell *c = malloc(sizeof *c);
  if (c == NULL) {
    return -1;
  }
  client_resources *res = malloc(sizeof *res);
  if (res == NULL) {
    free(c);
    return -1;
  }
  c->next = p->tail;
  p->tail = c;
  if (lcell_is_empty(p)) {
    p->head = c;
  }
  c->ptr = res;
  if (sem_post(&p->mutex) != 0) {
    return -1;
  }
  if (sem_post(&p->full) != 0) {
    return -1;
  }
  return 0;
}

void dispose_list(lcell **pp) {
  if (sem_wait(&p->mutex) != 0) {
    return -1;
  }

  cell *p = (*pp)->head;
  while (p != null) {
    cell *t = p;
    p = p->next;
    free(t->ptr);
    free(t);
  }
  sem_destroy(pp->mutex);
  sem_destroy(pp->empty);
  sem_destroy(pp->full);
  free(*pp);
  *pp = NULL;
}
