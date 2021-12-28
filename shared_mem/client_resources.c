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
#include <math.h>
#include "client_resources.h"

#define PIPE_SIZE_NAME 40

struct client_resources {
  char pipe_request[PIPE_SIZE_NAME];
  char pipe_response[PIPE_SIZE_NAME];
};

#define NAME_SHM "/client_resources_"
#define SIZE_SHM sizeof(client_resources)


client_resources *client_resources_empty(pid_t pid) {
    size_t pid_length = (size_t) ((ceil(log10(pid)) + 1) * sizeof(char));
    char name_shm[18 + pid_length + 1];
    snprintf(name_shm, 18 + pid_length + 1, "%s_%d", NAME_SHM, pid);

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


// Initialiser les pipe

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
