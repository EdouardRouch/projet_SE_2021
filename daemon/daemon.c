#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <syslog.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <pthread.h>
#include "daemon.h"
#include "../shared_mem/shared_fifo.h"
#include "../shared_mem/client_resources.h"

#define MUL 100
#define BUF_SIZE 256

void handler(int num);

fifo *p;

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
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGINT);
        sigdelset(&mask, SIGTERM);
        sigprocmask(SIG_SETMASK, &mask, NULL);

        struct sigaction sa;
        sa.sa_handler = handler;
        sigfillset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGTERM, &sa, NULL);


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

            p = fifo_empty();

            if (p == NULL) {
              fprintf(stderr, "erreur\n");
            }

            while (1) {
              printf("next_request");
              pid_t pid = fifo_next_request(p);
              client_resources *clr = client_resources_create(pid);


              pthread_t th;
              if (pthread_create (&th , NULL , treat_request , clr) != 0) {
                fprintf (stderr , " Erreur \n");
                exit( EXIT_FAILURE );
              }

              if (pthread_join(th, NULL) == -1 ) {
                perror("pthread_join");
                exit(EXIT_FAILURE);
              }

            }
            break;

        default:
            exit(EXIT_SUCCESS);
        }
        break;

    default:
        exit(EXIT_SUCCESS);

    }
}

void * treat_request(void * arg) {


  client_resources *clr = (client_resources *) arg;
  int fd_request = open(clr->pipe_request, O_RDONLY);
  if (fd_request == -1) {
    perror("open");
  }
  int fd_response = open(clr->pipe_response, O_WRONLY);
  if (fd_request == -1) {
    perror("open");
  }

  while (1) {
    char request[BUF_SIZE];
    ssize_t n = read(fd_request, request, BUF_SIZE - 1);

    if (n == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    // printf("%s\n", request);
    if (write(fd_response, request,(size_t) n) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
    // char buffer[BUF_SIZE][BUF_SIZE];
    char *buffer[256];
    char *token;
    char *rest = request;
    size_t c = 0;
    while ((token = strtok_r(rest, " ", &rest)) != NULL) {
        // printf("test : %s\n", token);
        buffer[c] = malloc(sizeof(token) + 2);
        if (buffer[c] == NULL) {
          perror("malloc");
          exit(EXIT_FAILURE);
        }
        strncpy(buffer[c], token, sizeof(token) + 1);
        ++c;
    }
    buffer[c] = NULL;
    // for (size_t i = 0; i <= c; i++) {
    //       printf("%s\n", buffer[i]);
    // }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
      // if(close(fd_request) == -1) {
      //   perror("close");
      //   exit(EXIT_FAILURE);
      // }
      // if (write(fd_response, request, sizeof(request)) == -1){
      //   perror("write");
      //   exit(EXIT_FAILURE);
      // }
      if (dup2(fd_response, STDOUT_FILENO) == -1) {
          perror("dup2");
          exit(EXIT_FAILURE);
      }
      // Rajouter log.txt pour stderr ?
      if (dup2(fd_response, STDERR_FILENO) == -1) {
          perror("dup2");
          exit(EXIT_FAILURE);
      }
      // printf("Coucou bande de nouilles \n");
      execvp(buffer[0], buffer);
      perror("execv");
      break;

    default:
      if (wait(NULL) == -1) {
        perror("wait");
        exit(EXIT_FAILURE);
      }

      for (size_t i = 0; i <= c; i++) {
            free(buffer[i]);
      }
      break;
    }
  }

  // close(fd_request);
  // close(fd_response);

  return NULL;
}


void handler(int num) {
	switch(num) {
		case SIGINT :
  			dispose_fifo(&p);
			exit(EXIT_SUCCESS);
		case SIGTERM :
  			dispose_fifo(&p);
			exit(EXIT_SUCCESS);
	}
}
