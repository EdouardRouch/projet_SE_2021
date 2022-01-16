#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif

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
#include <ctype.h>
#include "daemon.h"
#include "../shared_mem/shared_fifo.h"
#include "../shared_mem/client_resources.h"
#include "../cmd/cmd.h"

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
    return fd;
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


            struct sigaction sa;
            sa.sa_handler = handler;
            sigfillset(&sa.sa_mask);
            sa.sa_flags = 0;
            sigaction(SIGINT, &sa, NULL);
            sigaction(SIGTERM, &sa, NULL);
            sigaction(SIGPIPE, &sa, NULL);


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
                    pid_t pid = fifo_next_request(p);
                    client_resources *clr = client_resources_create(pid);

                    pthread_t th;
                    if (pthread_create (&th , NULL , treat_request , clr) != 0) {
                        fprintf (stderr , " Erreur \n");
                        exit( EXIT_FAILURE );
                    }
                    if (pthread_detach(th) == -1 ) {
                        perror("pthread_detach");
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

void *treat_request(void * arg) {
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
        request[n] = '\0';
        if (n == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (n == 0) {
            goto exit;
        }

        char *buffer[256];
        char *token;
        char *rest = request;
        size_t c = 0;
        while ((token = strtok_r(rest, " ", &rest)) != NULL) {
            buffer[c] = malloc(sizeof(token) + 2);
            if (buffer[c] == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            strncpy(buffer[c], token, sizeof(token) + 1);
            ++c;
        }
        buffer[c] = NULL;
        switch (fork()) {
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);

            case 0:
                if (dup2(fd_response, STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd_response, STDERR_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }

                if (strcmp("info_proc", buffer[0]) == 0) {
                    if (isdigit(buffer[1][0]) != 0) {
                        pid_t pid = (pid_t) atol(buffer[1]);
                        info_proc(pid);
                    } else {
                        fprintf(stderr, "Arguments invalide pour %s\n"
                                , buffer[0]);
                        exit(EXIT_FAILURE);
                    }
                } else if (strcmp("info_user", buffer[0]) == 0) {
                    if (isdigit(buffer[1][0]) != 0) {
                        uid_t uid = (uid_t) atol(buffer[1]);
                        info_user_uid(uid);
                    } else if (isalpha(buffer[1][0]) != 0) {
                        info_user_name(buffer[1]);
                    } else {
                        fprintf(stderr, "Arguments invalide pour %s\n"
                                , buffer[0]);
                        exit(EXIT_FAILURE);
                    }
                } else {
                    execvp(buffer[0], buffer);
                    perror("execv");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
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
    exit:
    close(fd_request);
    close(fd_response);

    return NULL;
}


void handler(int num) {
    switch(num) {
        case SIGINT:
            dispose_fifo(&p);
            exit(EXIT_SUCCESS);
        case SIGTERM:
            dispose_fifo(&p);
            exit(EXIT_SUCCESS);
        case SIGPIPE:
            dispose_fifo(&p);
            pthread_exit(NULL);
    }
}
