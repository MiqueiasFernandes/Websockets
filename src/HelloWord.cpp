//============================================================================
// Name        : HelloWord.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "server.h"
#include "thread.h"

using namespace std;

#include <cstdlib>
#include <pthread.h>

#define NUM_CLIENTES     5
#define PORTA_INICIAL 7000

int main() {
	cout << "!!!WEBSOCKETS!!!" << endl;

	int ini = PORTA_INICIAL;

	cout << "Suporte a " << NUM_CLIENTES << " porta: " << ini << " a "
			<< ini + NUM_CLIENTES - 1 << endl;

	cout << "> iniciando servidor..." << endl;

	int sockets[NUM_CLIENTES], qt_clientes = 0;

	for (int var = 0; var < NUM_CLIENTES; ++var) {
		sockets[var] = -1;
	}

	data dados;

	dados.porta = PORTA_INICIAL;
	dados.conectados = &qt_clientes;
	dados.max_clientes = NUM_CLIENTES;
	dados.sockets = sockets;

	server(&dados);

	return 0;
}
