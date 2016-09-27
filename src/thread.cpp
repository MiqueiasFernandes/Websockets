/*
 * thread.cpp
 *
 *  Created on: 7 de mai de 2016
 *      Author: mfernandes
 */

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <string.h>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include "funcoes.h"
#include "server.h"
#include "thread.h"

using namespace std;

void *PrintHello(void *threadarg) {

	data *args;

	args = (data *) threadarg;

	data dados = *args;

//data.porta = 5556;
//	data.id = 0;
//	int portas[5];
//	data.minhaporta = portas;
//	data.num_clientes = 5;

	cout << "> thread cliente " << dados.id << " rodando na porta "
			<< dados.porta << "..." << endl;

	server(args);

	pthread_exit(NULL);
}

int _thread(data* td) {
	pthread_t thread;

	int rc;

//	td->porta = porta;
//	td->id = id;
//	td->minhaporta = minha_porta;
//	td->num_clientes = num_clientes;

	cout << "> criando thread para cliente " << td->id << " porta " << td->porta
			<< endl;

	rc = pthread_create(&thread, NULL, PrintHello, (void *) td);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		return 0;
	}
	return 1;
}

