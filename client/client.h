#ifndef CLIENT__H
#define CLIENT__H

#include "shared_fifo.h"
#include "client_resources.h"

// open_pipe_request : ouvre le tube de requête alloué au client, renvoie le
//     descripteur de fichier sinon -1
extern int open_pipe_request(client_resources *clr);

// open_pipe_response :  ouvre le tube de réponse alloué au client, renvoie le
//     descripteur de fichier sinon -1
extern int open_pipe_response(client_resources *clr);

// send_request : envoi la requête du client à travers le tube de requête
extern int send_request(const char *request, int fd_request);

// receive_response : récupère la réponse à la requête précédente à travers
//    le tube de réponse
extern void receive_response(int fd_response);

#endif
