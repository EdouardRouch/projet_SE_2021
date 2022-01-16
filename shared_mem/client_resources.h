#ifndef CLIENT_RESOURCES__H
#define CLIENT_RESOURCES__H

#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>

#define PIPE_REQ_NAME "pipe_request_"
#define PIPE_RES_NAME "pipe_response_"
#define PIPE_MAX_SIZE_NAME 30

typedef struct client_resources client_resources;

struct client_resources {
    pid_t pid;
    char pipe_request[PIPE_MAX_SIZE_NAME];
    char pipe_response[PIPE_MAX_SIZE_NAME];
};

#define NAME_SHM "/client_resources_"
#define SIZE_SHM sizeof(client_resources)


// client_resources_empty : creation d'un objet de type client_resources dans un
//    espace de memoire partagee avec pour suffixe du nom le pid du client
extern client_resources *client_resources_create(pid_t pid);

// client_resources_get : renvoie un pointeur sur l'objet client_resources avec
//    comme suffixe pid dans la memoire partagee si celui existe
//    sinon renvoie null
extern client_resources *client_resources_get(pid_t pid);

// client_resources_dispose : libération des ressources allouées à *pp
extern void client_resources_dispose(client_resources **pp, pid_t pid);

#endif
