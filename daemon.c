#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include "daemon.h"

int init_daemon() {

    switch(fork()){
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
        if (setsid(0) == -1) {
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
            chdir("/tmp");

            // Ferme tous les descripteurs de fichiers éventuellement ouverts
            for (int i = sysconf(_SC_OPEN_MAX); i>=0; i--) {
                if (close(i) == -1) {
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