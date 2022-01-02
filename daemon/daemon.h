#ifndef CLIENT__H
#define CLIENT__H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#include "../shared_mem/client_resources.h"


void init_daemon();

// open_pipe_request : ouvre le tube de requête alloué au client, renvoie le
//     descripteur de fichier sinon -1
extern int open_pipe_request(client_resources *clr);

// open_pipe_response :  ouvre le tube de réponse alloué au client, renvoie le
//     descripteur de fichier sinon -1
extern int open_pipe_response(client_resources *clr);


extern int send_response(const char *response, int fd_response);

extern void receive_request(int fd_request);

extern void * treat_request(void * arg);

#endif
