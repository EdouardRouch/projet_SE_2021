#include <stdio.h>
#include <unistd.h>
#include "client.h"
#include "shared_fifo.h"
#include "client_resources.h"


int main(void) {
  fifo *p = fifo_get();
  if (p == NULL) {
    printf("erreur\n");
    exit(EXIT_FAILURE);
  }

  pid_t pid = getpid();
  fifo_request(p, pid);
  while (getchar() != EOF);


  return 0;
}

//
// int send_request(const char *request, client_resources *clr) {
//   return 0;
// }
