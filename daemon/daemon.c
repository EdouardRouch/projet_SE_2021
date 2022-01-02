#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <pthread.h>
#include "daemon.h"
#include <sys/wait.h>
#include "../shared_mem/shared_fifo.h"
#include "../shared_mem/client_resources.h"

#define MUL 100
#define BUF_SIZE 4096

int main(void) {

  init_daemon();

  return EXIT_SUCCESS;
}


int open_pipe_request(client_resources *clr) {
  int fd = open(clr->pipe_request, O_RDONLY);
  return fd;
}

int open_pipe_response(client_resources *clr) {
  int fd = open(clr->pipe_response, O_WRONLY);
  // unlink(clr->pipe_response);
  return fd;
}

int send_response(const char *response, int fd_response) {
  // for (size_t i = 0; i <= sizeof(fd_response); ++i) {
  //   if (write(fd_response, response + i, 1) == -1) {
  //     return -1;
  //   }
  // }
    if (write(fd_response, response, sizeof(response)) == -1) {
      return -1;
    }


  return 0;
}

void receive_request(int fd_request) {
  // char c;
  // ssize_t n = 1;
  // while ((n = read(fd_request, &c, 1)) > 0) {
  //     printf("%c", c);
  // }
  // printf("\n");
  char buffer[50];
  if(read(fd_request, buffer, sizeof(buffer)) == -1) {
    printf("Erreur lecture\n");
  }
  printf("%s\n", buffer);

}


void init_daemon() {

    switch(fork()){
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
        if (setsid() == -1) {
            perror("setsid");
            exit(EXIT_FAILURE);
        }

        signal(SIGHUP, SIG_IGN);
        /*TODO: implémenter la gestion des signaux */

        switch (fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0:
            umask(0);

            // Change le répertoire de travail du démon pour /tmp
            if(chdir("/tmp") == -1) {
              perror("chdir");
              exit(EXIT_FAILURE);
            }

            fifo *p = fifo_empty();
            if (p == NULL) {
              fprintf(stderr, "erreur\n");
            }

            while (1) {
              pid_t pid = fifo_next_request(p);
              printf("pid : %u \n", pid);
              client_resources *clr = client_resources_create(pid);

              pthread_t th;
              if (pthread_create (&th , NULL , treat_request , clr) != 0) {
                fprintf (stderr , " Erreur \n");
                exit( EXIT_FAILURE );
              }
            }

        default:
            exit(EXIT_SUCCESS);
        }

    default:
        exit(EXIT_SUCCESS);

    }
}

void * treat_request(void * arg) {
  client_resources *clr = (client_resources *) arg;
  int fd_request = open_pipe_request(clr);
  if (fd_request == -1) {
    perror("open");
  }
  int fd_response = open_pipe_response(clr);
  if (fd_request == -1) {
    perror("open");
  }
  printf("Ressources allouées ! \n");
  
  while (1) {
    char *request[BUF_SIZE];
    while (read(fd_request, request, PIPE_BUF) > 0);

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    
    case 0:
        if (dup2(fd_response, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        execv(request[0], request);
        break;
    default:
      if (wait(NULL) == -1) {
        perror("wait");
        exit(EXIT_FAILURE);
      }

      close(fd_request);
      close(fd_response);
      break;
    }
  }
  
}
