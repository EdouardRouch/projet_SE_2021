#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif

#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "shared_list.h"

#define LIST_SIZE 20
#define NOM_SHM "/mon_shm_en_shared_list_589422985365427"
#define TAILLE_SHM sizeof(lcell)

struct client_resources {
  char *pipe_request;
  char *pipe_response;
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
  int shm_fd = shm_open(NOM_SHM, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open");
    return NULL;
  }

  if (ftruncate(shm_fd, TAILLE_SHM) == -1) {
    perror("ftruncate");
    return NULL;
  }
  shm_unlink(NOM_SHM);

  lcell *p = mmap(NULL, TAILLE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (p == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  if (close(shm_fd) == -1) {
    perror("close");
    return NULL;
  }
  p->head = NULL;
  p->tail = NULL;
  if (sem_init(&p->mutex, 1, 1) != 0) {
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

lcell *lcell_get_list(void) {
  int shm_fd = shm_open(NOM_SHM, O_RDWR, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open");
    return NULL;
  }
  shm_unlink(NOM_SHM);

  lcell *p = mmap(NULL, TAILLE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (p == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  if (close(shm_fd) == -1) {
    perror("close");
    return NULL;
  }
  return p;
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

  c->ptr = res;
  c->next = p->head;
  p->head = c;
  if (lcell_is_empty(p)) {
    p->tail = c;
  }

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

  c->ptr = res;
  c->next = NULL;
  if (lcell_is_empty(p)) {
    p->head = c;
  } else {
    p->tail->next = c;
  }
  p->tail = c;

  if (sem_post(&p->mutex) != 0) {
    return -1;
  }
  if (sem_post(&p->full) != 0) {
    return -1;
  }
  return 0;
}

void dispose_head(lcell **pp) {
  if (sem_wait(&(*pp)->full) != 0) {
    return;
  }
  if (sem_wait(&(*pp)->mutex) != 0) {
    return;
  }
  cell *t = (*pp)->head;
  (*pp)->head = (*pp)->head->next;
  free(t);
  if (sem_post(&(*pp)->mutex) != 0) {
    return;
  }
  if (sem_post(&(*pp)->empty) != 0) {
    return;
  }
}

void dispose_list(lcell **pp) {
  if (sem_wait(&(*pp)->mutex) != 0) {
    return;
  }

  cell *p = (*pp)->head;
  while (p != NULL) {
    cell *t = p;
    p = p->next;
    free(t->ptr);
    free(t);
  }
  sem_destroy(&(*pp)->mutex);
  sem_destroy(&(*pp)->empty);
  sem_destroy(&(*pp)->full);
  munmap(*pp, TAILLE_SHM);
  *pp = NULL;
}
