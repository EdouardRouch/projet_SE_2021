#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include "daemon.h"
#include "shared_list.h"


int main(void) {
  lcell *p = create_empty_list();
  if (p == NULL) {
    fprintf(stderr, "erreur\n");
  }
  lcell_insert_tail(p);

  char buf;
  while(scanf("%c", &buf) != EOF);

  if (lcell_is_empty(p)) {
    printf("Vide ! \n");
  } else {
    printf("Plein ! \n");
  }
  dispose_head(&p);

  if (lcell_is_empty(p)) {
    printf("Vide ! \n");
  } else {
    printf("Plein ! \n");
  }
  dispose_list(&p);


  return 0;
}




int init_daemon() {

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

            // Ferme tous les descripteurs de fichiers éventuellement ouverts
            for (long int i = sysconf(_SC_OPEN_MAX); i>=0; i--) {
                if (close((int) i) == -1) {
                    perror("close");
                    exit(EXIT_FAILURE);
                }
            }

            return getpid();

        default:
            exit(EXIT_FAILURE);
        }

    default:
        exit(EXIT_FAILURE);

    }
}
