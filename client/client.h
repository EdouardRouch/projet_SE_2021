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

// Envoi la requête du client à travers la ressource partagée client_resources
extern int send_request(const char *request, client_resources *clr);

#endif
