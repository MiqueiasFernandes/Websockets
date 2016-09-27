/*
 * server.h
 *
 *  Created on: 5 de mai de 2016
 *      Author: mfernandes
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "thread.h"

int server(data *dados);

void env_broadcast(char* msg, int* clientes, int max_clientes);

#endif /* SERVER_H_ */
