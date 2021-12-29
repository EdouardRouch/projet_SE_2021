#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include "client.h"
#include "shared_fifo.h"
#include "client_resources.h"

#define MUL 100

int main(void) {
  fifo *p = fifo_get();
  if (p == NULL) {
    printf("erreur\n");
    exit(EXIT_FAILURE);
  }

  pid_t pid = getpid();
  fifo_request(p, pid);
  while (getchar() != EOF);


  return 0;
}


int open_pipe_request(client_resources *clr) {
  int fd = open(clr->pipe_request, O_WRONLY);
  return fd;
}

int open_pipe_response(client_resources *clr) {
  int fd = open(clr->pipe_response, O_RDONLY);
  return fd;
}

int send_request(const char *request, int fd_request) {
  for (int i = 0; i < sizeof(request); ++i) {
    if (write(fd_request, request + i, 1) == -1) {
      return -1;
    }
  }
  return 0;
}

char *receive_response(int fd_response) {
  char buffer[2 * MUL * PIPE_BUF];
  ssize_t n;
  ssize_t pos = 0;
  while ((n = read(fd_response, buffer + pos, (size_t) (2 * MUL * PIPE_BUF - pos))) > 0) {
    pos += n;
  }
  return buffer;
}
