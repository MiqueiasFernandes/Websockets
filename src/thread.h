/*
 * thread.h
 *
 *  Created on: 7 de mai de 2016
 *      Author: mfernandes
 */

#ifndef THREAD_H_
#define THREAD_H_


typedef struct thread_data {
	int id;
	int* sockets;
	int max_clientes;
	int porta;
	int* conectados;
}data;

int _thread(data* dados);

#endif /* THREAD_H_ */
