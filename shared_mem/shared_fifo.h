#ifndef SHARED_FIFO__H
#define SHARED_FIFO__H

#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>

typedef struct client_resources client_resources;

typedef struct fifo fifo;

// fifo_empty : creation d'un objet de type fifo dans un espace
//    de memoire partagee
extern fifo *fifo_empty(void);

// fifo_get : renvoie un pointeur sur la file fifo dans la memoire partagee si
//    celui existe sinon renvoie null
extern fifo *fifo_get(void);

// fifo_request : demande l'allocation de ressources pour le client de pid pid
//    sur la file fifo p, renvoie 0 en cas de succès sinon -1
extern int fifo_request(fifo *p, pid_t pid);

// fifo_treat_request : récupère la prochaine requête à traitée du client pid
extern pid_t fifo_next_request(fifo *p);

// dispose_fifo : libération des ressources allouées à *p
extern void dispose_fifo(fifo **pp);


#endif
