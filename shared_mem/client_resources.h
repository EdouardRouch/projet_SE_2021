#ifndef CLIENT_RESOURCES__H
#define CLIENT_RESOURCES__H

#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>

typedef struct client_resources client_resources;

// client_resources_empty : creation d'un objet de type client_resources dans un
//    espace de memoire partagee avec pour suffixe du nom le pid du client
extern client_resources *client_resources_empty(pid_t pid);

// client_resources_get : renvoie un pointeur sur l'objet client_resources avec
//    comme suffixe pid dans la memoire partagee si celui existe
//    sinon renvoie null
extern client_resources *client_resources_get(pid_t pid);

#endif
