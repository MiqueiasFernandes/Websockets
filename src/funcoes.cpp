#define SOCKET int

using namespace std;

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include "sha.h"
#include "bmh.h"
#include "b64.h"

#define CHAR_BIT 8

////ESTA FUN��O CONVERTE CHAR PARA BINARIO
//
char* chartobin(unsigned char c) {
	static char bin[CHAR_BIT + 1] = { 0 };
	int i;

	for (i = CHAR_BIT - 1; i >= 0; i--) {
		bin[i] = (c % 2) + '0';
		c /= 2;
	}

	return bin;
}

//
////ESTA FUN��O CONVERTE BINARIO PARA CHAR
//
unsigned char bintochar(char* binstr) {
	return (unsigned char) (strtoul(binstr, NULL, 2));
}

//ESTA FUNÇÃO DECODIFICA DADOS BINARIOS GERADOS PELO CLIENTE WEBSOCKET
char* Decodificar_Binario(char* comparar) {


	cout << "> decodificando os dados recebidos..." << endl;

	char saida[100000];
	int i, k;

	//Verificar tamanho de Payload
	long unsigned int inicio = 6;
	long unsigned int pos_mask = 2;
	long unsigned int Payload_len = (int) bintochar(&chartobin(comparar[1])[1]); ///0-125
	if (Payload_len == 126) { ////////////////////////////////////////126-65535
		char binario[20];
		strcpy(binario, chartobin(comparar[2]));
		strcat(binario, chartobin(comparar[3]));
		short unsigned int x16 =
				(short unsigned int) (strtoul(binario, NULL, 2));
		Payload_len = (long unsigned int) x16;
		inicio = 8;
		pos_mask = 4;
	} else if (Payload_len == 127) { ////////////////////////////////////////65535-100.000
		char binario[70];
		strcpy(binario, chartobin(comparar[2]));
		for (k = 3; k < 10; k++)
			strcat(binario, chartobin(comparar[k]));
		long unsigned int x64 = (long unsigned int) (strtoul(binario, NULL, 2));
		if (x64 < 100000)
			Payload_len = x64;
		else {
			Payload_len = 99999;
			//	printf("#DADOS PERDIDOS: %i;\n", x64 - 99999);
		}
		inicio = 14;
		pos_mask = 4;
	}

	////Recuperar 4 Mascaras da resposta
	unsigned char mask[4], cod;
	for (i = 0; i < 4; i++)
		mask[i] = (unsigned char) comparar[pos_mask++];

	//aplicar mascaras na resposta
	for (pos_mask = 0; pos_mask < Payload_len; pos_mask++) {
		cod = mask[pos_mask % 4] ^ comparar[inicio++];
		saida[pos_mask] = (char) cod;
	}
	saida[pos_mask] = '\0';

	sprintf(comparar, "%s", saida);

	return comparar;
}

//ESTA FUNÇÃO PREPARA MENSAGEM PARA SER ENVIADA A UM CLIENTE WEBSOCKET
int converte(char* in) {

	cout << "> preparando dados a serem enviados..." << endl;
	char saida[200];
	int len = 2;
	len = sprintf(saida, "%c%c%s\n", bintochar((char *) &"10000001"),
			bintochar(chartobin(strlen(in))), in);
	strcpy(in, saida);
	in[len--] = '\0';
	return len;
}

//ESTA FUNÇÃO ENVIA MENSAGEM PARA WEBSOCKET
void enviar(const char* in, SOCKET soc) {
	char msg[128];
	int j = 0;
	if (strlen(in) < 126) {
		sprintf(msg, "%s", in);
		j = converte(msg);
	} else {
		printf("ALERT: TAMANHO DE MENSAGEM EXCEDIDO\n");
	}
	send(soc, msg, j, 0);
	printf("TX ENV: %s\n", in);
}

////ESTA FUN��O SELECIONA ENTRE AS CONEXOES ACEITAS:
////CASO HTTP: ENCAMINHA PARA SERVIDOR QUE CONECTA WEBSOCKET - RETORNO = VALIDADOR DA CHAVE
////CASO WEBSOCKET: CONECTA E PERMANECE CONECTADO - RETORNO = -1
////OUTRO CASO: FECHA A CONEX�O - RETORNO = 0
int Selecionar_Conexao(char *dado, int new_socket, int porta_websocket) {

	///caso websockets
	int i = buscar(dado, "Sec-WebSocket-Key") - 1;

	if (i > 0) {

		cout << "> recebemos uma conexao websocket..." << endl;

		char* strg = dado;
		char key[100];
		int j;

		//Obter Key de Handshake do cliente

		cout << "> linha do key: ";

		for (int var = i; dado[var] != '\n'; var++) {
			cout << dado[var];
		}

		cout << endl;

		if (!strncmp(&strg[i], "Sec-WebSocket-Key", 17)) {

			i += 19;
			j = i;
			while (strg[j] != '=') {
				key[j - i] = strg[j];
				j++;
			}
			key[j - i] = '\0';
			if (j != strlen(strg)) {
				if (strg[j + 1] == '=')
					strcat(key, (const char *) &"==");
				else
					strcat(key, (const char *) &"=");
				//printf("->%s<-\n", key);
			} else
				//Fecha a conex�o, � impossivel conectar
				return -1;
		}

		////RFC-6455 OBETER CHAVE DE ACESSO
		char saida[100000];
		unsigned char hash[20];
		unsigned char buf[1000];

		cout << "> chave: " << key << "..." << endl;

		int n = sprintf((char *) buf, "%s258EAFA5-E914-47DA-95CA-C5AB0DC85B11",
				key);

		cout << "> nova chave: " << buf << "..." << endl;

		SHA1(buf, n, hash);

		cout << "> SHA1: " << hash << "..." << endl;

		n = lws_b64_encode_string((char *) hash, 20, (char *) buf, 1000);

		cout << "> B64: " << buf << "..." << endl;

		cout << "> construindo handsahke..." << endl;
		//construindo Handshake
		sprintf(saida, "HTTP/1.1 101 Switching Protocols\x0d\x0a"
				"Upgrade: WebSocket\x0d\x0a"
				"Connection: Upgrade\x0d\x0a"
				"Sec-WebSocket-Accept: %s\x0d\x0a\x0d\x0a", buf);
		//	"Sec-WebSocket-Protocol: websockets\x0d\x0a\x0d\x0a", buf);

		//enviando Handshake
		send(new_socket, saida, strlen(saida), 0);
		/////aceitou conex�o

		cout << "> handsahke enviado:" << endl << saida << endl;

		return 1;

	}

	if (strncmp(dado, "GET / HTTP/1.1", 14) == 0) {

		cout << "> conexao http index..." << endl;

		cout << "> construindo pagina para "<< porta_websocket << "..." << endl;

		int j =0;
		 i = buscar(dado, "Host: ") + 5;
		char host[20];
		 while(dado[i] != ':')
		 {
			 host[j++] = dado[i++];
		 }
		 host[j++] = '\0';

		printf("host: %s\n",host);

		char msg[1000];

		sprintf(msg,
				"<html>\n\
<header><title>WebSockets %i</title><script>\n\
var ws = new WebSocket('ws://%s:%i');\n\
ws.onopen = function() {var title = document.getElementById('title'); title.style.color= 'green';\n\
title.innerHTML = '<b>WEBSOCKETS %i<b>';\n\
ws.send('conectado com sucesso!');};\n\
ws.onmessage = function(message) {\n\
document.getElementById('message').innerHTML += message.data + '<br>';\n\
};\n\
ws.onerror = function(evt) { var title = document.getElementById('title'); title.style.color= 'red';\n\
title.innerHTML = '<b>evt<b>';}\n\
function enviar(){ var campo = document.getElementById('text'); ws.send(campo.value); campo.value= ''}\n\</script></header>\n\
<body><h1><div id='title'>websockets</div></h1><br>\n\
Enviar: <input type='text' id='text' value='digite para enviar...''>\n\
<input type='button' value='ENVIAR' onclick='enviar()'><br><br>\n\
<br><div id='message'></div>\n\
</body></html>", porta_websocket, host, porta_websocket, porta_websocket);

		int len = strlen(msg);
		send(new_socket, msg, len, 0);

		cout << "> pagina enviada..." << endl;
		return 0;
	}

	//	///OUTROS PEDIDOS

	cout << "> tipo de conexao invalida, nao vamos responder..." << endl;

	return -1;
}
