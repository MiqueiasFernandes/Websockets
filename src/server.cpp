/*
 * server.cpp
 *
 *  Created on: 5 de mai de 2016
 *      Author: mfernandes
 */

#include "funcoes.h"
#include "thread.h"
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions

void env_broadcast(char* id, int socket, char* msg, int* clientes, int max_clientes);

int server(data* dados) {

	char p[8];
	sprintf(p, "%i", (int) (dados->porta));

	const char _porta[] = { p[0], p[1], p[2], p[3], '\0' };

	int status;
	struct addrinfo host_info; // The struct that getaddrinfo() fills up with data.
	struct addrinfo *host_info_list; // Pointer to the to the linked list of host_info's.

	// The MAN page of getaddrinfo() states "All  the other fields in the structure pointed
	// to by hints must contain either 0 or a null pointer, as appropriate." When a struct
	// is created in c++, it will be given a block of memory. This memory is not nessesary
	// empty. Therefor we use the memset function to make sure all fields are NULL.
	memset(&host_info, 0, sizeof host_info);

	cout << "> inicializando structs tcp para porta " << _porta << "..."
			<< endl;

	host_info.ai_family = AF_UNSPEC;   // IP version not specified. Can be both.
	host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
	host_info.ai_flags = AI_PASSIVE;     // IP Wildcard

	// Now fill up the linked list of host_info structs with google's address information.
	status = getaddrinfo(NULL, _porta, &host_info, &host_info_list);
	// getaddrinfo returns 0 on succes, or some other value when an error occured.
	// (translated into human readable text by the gai_gai_strerror function).
	if (status != 0)
		std::cout << "getaddrinfo error" << gai_strerror(status);

	cout << "> criando socket para porta " << _porta << "..." << endl;
	int socketfd; // The socket descripter
	socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
			host_info_list->ai_protocol);
	if (socketfd == -1)
		std::cout << "socket error ";

	cout << "> binding socket..." << endl;
	// we use to make the setsockopt() function to make sure the port is not in use
	// by a previous execution of our code. (see man page for more information)
	int yes = 1;
	status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	status = bind(socketfd, host_info_list->ai_addr,
			host_info_list->ai_addrlen);
	if (status == -1)
		std::cout << "bind error" << std::endl;

	cout << "> esperando conexoes..." << endl;
	status = listen(socketfd, 5);
	if (status == -1)
		std::cout << "listen error" << std::endl;

	int new_sd = -1;
	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof(their_addr);
	while (-1
			!= (new_sd = accept(socketfd, (struct sockaddr *) &their_addr,
					&addr_size))) {

		cout << "> conexao aceita..." << endl;

		cout << "> socket da conexao " << new_sd << "..." << endl;

		cout << "> aguardando dados..." << endl;

		ssize_t bytes_recieved;
		char incomming_data_buffer[100000];
		bytes_recieved = recv(new_sd, incomming_data_buffer, 100000, 0);
		// If no data arrives, the program will just wait here until some data arrives.
		if (bytes_recieved == 0)
			std::cout << "host shut down." << std::endl;
		if (bytes_recieved == -1)
			std::cout << "recieve error!" << std::endl;
		std::cout << bytes_recieved
				<< " bytes recebidos:---------------------------------------"
				<< std::endl;
		incomming_data_buffer[bytes_recieved] = '\0';
		std::cout << incomming_data_buffer << std::endl;
		cout << "-----------------------------------------" << endl;
		////imprimiu os dados recebidos

		cout << "> verificando tipo de conexao..." << endl;
		int temp = Selecionar_Conexao(incomming_data_buffer, new_sd,
				dados->porta + *(dados->conectados) + 1);

		if (temp == 0) {

			///abrir thread para websocket na nova porta

			data dado;
			dado.conectados = dados->conectados;
			dado.id = *(dados->conectados);
			dado.max_clientes = dados->max_clientes;
			dado.porta = dados->porta + *(dados->conectados) + 1;
			dado.sockets = dados->sockets;

			cout << "> abrindo thread para cliente " << dado.id << endl;

			_thread(&dado);

		}

		if (temp <= 0) {

			cout << "> fechando socket..." << endl;

			shutdown(new_sd, 2);

			cout << "> aguardando novas conexoes..." << endl;

			continue;
		}

		if (temp == 1) {

			//caso é websocket

			dados->sockets[dados->id] = new_sd;

			(*(dados->conectados))++;

			cout << "> " << *(dados->conectados)
					<< " conexao websocket aberta id " << dados->id
					<< " socket " << dados->porta << "..." << endl;
			char msg[10000], id[50];

			sprintf(msg, "cliente %i - %i acabou de entrar!", dados->id,
					dados->porta);

			env_broadcast(id, dados->id, msg, (dados->sockets),
					*(dados->conectados));

			sprintf(id, "mensagem de cliente %i - %i :", dados->id,
								dados->porta);

			while (recv(new_sd, incomming_data_buffer, 100000, 0) != -1)

			{

				cout << "> mensagem recebida!..." << endl;

				char* msg = Decodificar_Binario(incomming_data_buffer);

				cout << "> MSG RECV: " << msg << endl;

				env_broadcast(id, new_sd, msg, (dados->sockets),
						*(dados->conectados));

//				if (strncmp(msg, "conectado com sucesso!", 22) == 0) {
//					env_broadcast(msg, portas, num_clientes);
//					cout << "> aguardando mensagem..." << endl;
//					continue;
//				}
//
//				cout << "> DIGITE SUA MENSAGEM:" << endl;
//
//				string tmp;
//
//				getline(cin, tmp);
//
//				cout << "> enviando sua mensagem..." << endl;
//				env_broadcast(msg, portas, num_clientes);
//				cout << "> aguardando mensagem..." << endl;

			}

			dados->sockets[dados->id] = -1;

			shutdown(new_sd, 2);

			cout << "> aguardando uma nova conexao..." << endl;

		}
	}

	cout << "> desligando servidor..." << endl;
	freeaddrinfo(host_info_list);

	return 0;

}

void env_broadcast(char * id, int socket, char* msg, int* clientes, int max_clientes) {
	int temp = -1;

	char tmp[1000];

	sprintf(tmp, "%s %s", id, msg);

	for (int var = 0; var < max_clientes; ++var) {

		temp = clientes[var];

		if (socket == temp || temp == -1)
			continue;

		enviar(tmp, temp);

	}
	sprintf(tmp, "<b>EU: %s<b>", msg);
	enviar(tmp, socket);
}

