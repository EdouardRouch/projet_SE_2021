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
#define BUF_SIZE 256

void handler(int num);


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
fifo *p;

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
        sigset_t masque;
        sigfillset(&masque);
        sigdelset(&masque, SIGINT);
        sigdelset(&masque, SIGTERM);
        sigprocmask(SIG_SETMASK, &masque, NULL);

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

            // fifo *p = fifo_empty();
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
    char *request[BUF_SIZE];
    ssize_t n = read(fd_request, request, BUF_SIZE - 1);
    if (n == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    if (write(fd_response, request, sizeof(request)) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }

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
      if (dup2(STDOUT_FILENO, fd_response) == -1) {
          perror("dup2");
          exit(EXIT_FAILURE);
      }


      printf("Coucou bande de nouilles \n");
      execv(request[0], request);
      perror("execv");
      break;

    default:
      if (wait(NULL) == -1) {
        perror("wait");
        exit(EXIT_FAILURE);
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
