#ifndef CLIENT__H
#define CLIENT__H

#include "shared_fifo.h"

// Envoi la requête du client à travers la ressource partagée client_resources
int send_request(const char *request, client_resources *clr);

#endif
