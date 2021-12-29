#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "client_resources.h"


// #define PIPE_REQ_NAME "pipe_request_"
// #define PIPE_RES_NAME "pipe_response_"
// #define PIPE_MAX_SIZE_NAME 30
//
// struct client_resources {
//   pid_t pid;
//   char pipe_request[PIPE_MAX_SIZE_NAME];
//   char pipe_response[PIPE_MAX_SIZE_NAME];
// };

#define NAME_SHM "/client_resources_"
#define SIZE_SHM sizeof(client_resources)

void initialize_char_name(client_resources *p);

client_resources *client_resources_create(pid_t pid) {
    size_t pid_length = (size_t) ((ceil(log10(pid)) + 1) * sizeof(char));
    char name_shm[sizeof(NAME_SHM) + pid_length + 1];
    snprintf(name_shm, sizeof(NAME_SHM) + pid_length + 1, "%s_%d", NAME_SHM, pid);

    int shm_fd = shm_open(name_shm, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
      perror("shm_open");
      return NULL;
    }

    if (ftruncate(shm_fd, SIZE_SHM) == -1) {
      perror("ftruncate");
      return NULL;
    }

    client_resources *p = mmap(NULL, SIZE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (p == MAP_FAILED) {
      perror("mmap");
      return NULL;
    }

    p->pid = pid;

    initialize_char_name(p);
    char name_pipe_req[sizeof(PIPE_REQ_NAME) + pid_length + 1];
    snprintf(name_pipe_req, sizeof(PIPE_REQ_NAME) + pid_length + 1, "%s_%d", PIPE_REQ_NAME, pid);
    if (mkfifo(name_pipe_req, S_IRUSR | S_IWUSR) == -1) {
      perror("mkfifo");
      exit(EXIT_FAILURE);
    }

    strncpy(p->pipe_request, name_pipe_req, sizeof(PIPE_REQ_NAME) + pid_length + 1);

    char name_pipe_res[sizeof(PIPE_RES_NAME) + pid_length + 1];
    snprintf(name_pipe_res, sizeof(PIPE_RES_NAME) + pid_length + 1, "%s_%d", PIPE_RES_NAME, pid);
    if (mkfifo(name_pipe_res, S_IRUSR | S_IWUSR) == -1) {
      perror("mkfifo");
      exit(EXIT_FAILURE);
    }
    strncpy(p->pipe_response, name_pipe_res, sizeof(PIPE_RES_NAME) + pid_length + 1);

    if (close(shm_fd) == -1) {
      perror("close");
      return NULL;
    }

    return p;
}

client_resources *client_resources_get(pid_t pid) {
  size_t pid_length = (size_t) ((ceil(log10(pid)) + 1) * sizeof(char));
  char name_shm[18 + pid_length + 1];
  snprintf(name_shm, 18 + pid_length + 1, "%s_%d", NAME_SHM, pid);

  int shm_fd = shm_open(name_shm, O_RDWR, S_IRUSR | S_IWUSR);
  if (shm_fd == -1) {
    perror("shm_open");
    return NULL;
  }

  if (ftruncate(shm_fd, SIZE_SHM) == -1) {
    perror("ftruncate");
    return NULL;
  }

  client_resources *p = mmap(NULL, SIZE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED
                                 , shm_fd, 0);
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

void client_resources_dispose(client_resources **pp) {
    munmap(*pp, SIZE_SHM);
    *pp = NULL;
}

void initialize_char_name(client_resources *p) {
  for (size_t i = 0; i < PIPE_MAX_SIZE_NAME; i++) {
    p->pipe_request[i] = '\0';
    p->pipe_response[i] = '\0';
  }
}
