#ifndef SHARED_LIST__H
#define SHARED_LIST__H

#include <stdlib.h>
// #include <semaphore.h>

typedef struct lcell lcell;


// lcell_empty : creation d'un objet de type lcell, une liste simplement chaînée
extern lcell *lcell_empty(void);

// lcell_is_empty : renvoie true ou false selon si la liste associée à p
//   est vide ou non
extern bool lcell_is_empty(const lcell *p);

// lcell_insert_head : insère en tête de la liste pointé par p un cellule cell
extern int lcell_insert_head(lcell *p);

// lcell_insert_head : insère en queue de la liste pointé par p un cellule cell
extern int lcell_insert_tail(lcell *p);

// lcell_allocate_head ; alloue les ressources pour les deux tubes de la cellule
//    de tête
extern int lcell_allocate_head(lcell *p);

// Libération des ressources allouées à *p ainsi que la liste associé
//    et ses sémaphores
extern  void dispose_list(lcell **p);



#endif
