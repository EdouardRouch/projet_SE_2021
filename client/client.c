#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <string.h>
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
        fprintf(stderr, "Erreur : Daemon indisponible.\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = getpid();
    fifo_request(p, pid);

    printf("Ouverture du client...\n");
    printf("Connexion au démon...\n");
    sleep(1);
    client_resources *clr = client_resources_get(pid);
    printf("Client\nrequest: %s\nresponse : %s\n", clr->pipe_request, clr->pipe_response);

    int fd_request = open_pipe_request(clr);
    if (fd_request == -1) {
        perror("open");
    }

    int fd_response = open_pipe_response(clr);
    if (fd_response == -1) {
        perror("open");
    }
    printf("Connexion au démon établie !\n\n");

    while (1) {
        printf("Tapez votre commande:\n");
        char buffreq[256];
        char buffres[4000];
        if (fgets(buffreq, 255, stdin)  == NULL) {
            goto leave;
        }
        buffreq[strlen(buffreq) - 1] = '\0';

        if (write(fd_request, buffreq, sizeof(buffreq)) == -1){
            perror("write");
            exit(EXIT_FAILURE);
        }
        printf("Requête correctement envoyée.\n");
        ssize_t n;
        if ((n = read(fd_response, buffres, sizeof(buffres))) < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        buffres[n] = '\0';
        printf("Réponse : %s \n", buffres);
    }
leave:
    if (!feof(stdin)) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }
    printf("Au revoir !\n" );
    close(fd_request);
    close(fd_response);


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
