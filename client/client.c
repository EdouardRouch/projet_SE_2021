#include <stdio.h>
#include "client.h"
#include "shared_list.h"


int main(void) {
  lcell *p = lcell_get_list();
  if (p == NULL) {
    printf("erreur\n");
    exit(EXIT_FAILURE);
  }
  // if (lcell_is_empty(p)) {
  //   printf("Vide ! \n");
  // } else {
  //   printf("Plein ! \n");
  // }


  return 0;
}

//
// int send_request(const char *request, client_resources *clr) {
//   return 0;
// }
