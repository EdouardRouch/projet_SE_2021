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
#include "shared_fifo.h"


#define PIPE_SIZE_NAME 40

struct client_resources {
  char pipe_request[PIPE_SIZE_NAME];
  char pipe_response[PIPE_SIZE_NAME];
};

#define SIZE_BUFFER 20
#define NAME_FIFO "/mon_shm_fifo_589422985365427"
#define SIZE_FIFO sizeof(fifo) + SIZE_BUFFER * sizeof(pid_t)

struct fifo {
  sem_t mutex;
  sem_t empty;
  sem_t full;
  int head;
  int tail;
  pid_t buffer[];
};

fifo *fifo_empty(void) {
  int shm_fd = shm_open(NAME_FIFO, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open");
    return NULL;
  }

  if (ftruncate(shm_fd, SIZE_FIFO) == -1) {
    perror("ftruncate");
    return NULL;
  }

  fifo *p = mmap(NULL, SIZE_FIFO, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (p == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  if (close(shm_fd) == -1) {
    perror("close");
    return NULL;
  }

  p->head = 0;
  p->tail = 0;
  if (sem_init(&p->mutex, 1, 1) != 0) {
    return NULL;
  }
  if (sem_init(&p->empty, 1, SIZE_BUFFER) != 0) {
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

fifo *fifo_get(void) {
  int shm_fd = shm_open(NAME_FIFO, O_RDWR, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    return NULL;
  }

  fifo *p = mmap(NULL, SIZE_FIFO, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
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

int fifo_request(fifo *p, pid_t pid) {
  if (sem_wait(&p->empty) != 0) {
    return -1;
  }
  if (sem_wait(&p->mutex) != 0) {
    return -1;
  }

  p->buffer[p->head] = pid;
  p->head = (p->head + 1) % SIZE_BUFFER;

  if (sem_post(&p->mutex) != 0) {
    return -1;
  }
  if (sem_post(&p->full) != 0) {
    return -1;
  }

  return 0;
}

pid_t fifo_next_request(fifo *p) {
  if (sem_wait(&p->full) != 0) {
    return -1;
  }
  if (sem_wait(&p->mutex) != 0) {
    return -1;
  }

  pid_t pid = p->buffer[p->tail];
  p->tail = (p->tail + 1) % SIZE_BUFFER;

  if (sem_post(&p->mutex) != 0) {
    return -1;
  }
  if (sem_post(&p->empty) != 0) {
    return -1;
  }

  return pid;
}

void dispose_fifo(fifo **pp) {
  if (sem_wait(&(*pp)->mutex) != 0) {
    return;
  }
  sem_destroy(&(*pp)->mutex);
  sem_destroy(&(*pp)->empty);
  sem_destroy(&(*pp)->full);
  shm_unlink(NAME_FIFO);
  munmap(*pp, SIZE_FIFO);
  *pp = NULL;
}
