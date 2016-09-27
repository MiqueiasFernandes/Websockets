#ifndef funcoes_H_
#define funcoes_H_

#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions

using namespace std;

#define SOCKET int

int Selecionar_Conexao(char *dado, int new_socket, int porta_websocket);

char* Decodificar_Binario(char* comparar);

int VALIDAR(char *CODIGO_PARA_LICENCA, int CHAVE_DE_LICENCA, SOCKET new_socket);

void enviar(const char* in, SOCKET soc);

#endif
