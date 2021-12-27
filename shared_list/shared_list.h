#ifndef SHARED_LIST__H
#define SHARED_LIST__H

#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>

typedef struct lcell lcell;

typedef struct client_resources client_resources;

// lcell_empty : creation d'un objet de type lcell, une liste simplement chaînée
//    dans un espace de memoire partagee
extern lcell *create_empty_list(void);

// lcell_is_empty : renvoie true ou false selon si la liste associee à p
//     est vide ou non
extern bool lcell_is_empty(const lcell *p);

// lcell_get_list : renvoie un pointeur sur la liste dans la memoire partagee si
//    celui n'est pas vide sinon renvoie null
extern lcell *lcell_get_list(void);

// lcell_insert_head : insere en tete de la liste pointe par p un cellule cell
extern int lcell_insert_head(lcell *p);

// lcell_insert_head : insere en queue de la liste pointe par p un cellule cell
extern int lcell_insert_tail(lcell *p);

// lcell_allocate_head : alloue les ressources pour les deux tubes de la cellule
//    de tête
extern int lcell_allocate_head(lcell *p);

// dispose_head : liberation des ressources lies à la premiere cellule de la
//    liste associe a *p
extern void dispose_head(lcell **p);

// dispose_list : liberation des ressources allouees a *p ainsi que la liste
//    associe et ses semaphores
extern void dispose_list(lcell **p);

// client_resources_init : initialise les tubes nommes de l'objet pointe par p
extern int client_resources_init(client_resources *p);

#endif
