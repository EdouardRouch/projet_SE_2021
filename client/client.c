#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/limits.h>
#include "client.h"
#include "shared_fifo.h"
#include "client_resources.h"

#define MUL 100

int main(void) {
  if(chdir("/tmp") == -1) {
    perror("chdir");
    exit(EXIT_FAILURE);
  }
  fifo *p = fifo_get();
  if (p == NULL) {
    printf("erreur\n");
    exit(EXIT_FAILURE);
  }

  pid_t pid = getpid();
  fifo_request(p, pid);

  printf("Requête bien demandée ! \n");
  while (getchar() != EOF);

  client_resources *clr = client_resources_get(pid);

  int fd_request = open_pipe_request(clr);
  if (fd_request == -1) {
    perror("open");
  }

  int fd_response = open_pipe_response(clr);
  if (fd_response == -1) {
    perror("open");
  }
  printf("Connection aux tubes terminée !\n");

  send_request("Bonjour ! \n", fd_request);
  printf("Requêtes envoyées ! \n");
  while (getchar() != EOF);

  printf("Réponse : ");
  receive_response(fd_response);

  printf("Au revoir !\n" );
  close(fd_request);
  close(fd_response);

  while (getchar() != EOF);


  return 0;
}


int open_pipe_request(client_resources *clr) {
  int fd = open(clr->pipe_request, O_WRONLY);
  // unlink(clr->pipe_response);
  return fd;
}

int open_pipe_response(client_resources *clr) {
  int fd = open(clr->pipe_response, O_RDONLY);
  return fd;
}

int send_request(const char *request, int fd_request) {
  // for (size_t i = 0; i <= sizeof(request) ; ++i) {
  //   if (write(fd_request, request + i, 1) == -1) {
  //     return -1;
  //   }
  // }

  if (write(fd_request, request, sizeof(request)) == -1) {
    return -1;
  }

  return 0;
}

void receive_response(int fd_response) {
  // char buffer[50];
  // if(read(fd_response, buffer, sizeof(buffer)) == -1) {
  //   printf("Erreur lecture\n");
  // }
  // printf("%s\n", buffer);
  char c;
  ssize_t n = read(fd_response, &c, 1);
  printf("%c", c);

  while ((n = read(fd_response, &c, 1)) > 0) {
      printf("%c", c);
  }
  printf("\n");
}
