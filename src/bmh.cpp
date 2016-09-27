/*
 * bmh.cpp
 *
 *  Created on: 6 de mai de 2016
 *      Author: mfernandes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
using namespace std;

#define MAXCHAR 256

int BMH(char *T, int n, char *P, int m) {
	int i, j, k;
	int d[MAXCHAR + 1];

	//pré-processamento
	for (j = 0; j <= MAXCHAR; j++) {
		d[j] = m;
	}
	for (j = 1; j < m; j++) {
		d[P[j - 1]] = m - j;
	}
	i = m;
	//busca
	while (i <= n) {
		k = i;
		j = m;
		while (T[k - 1] == P[j - 1] && j > 0) {
			k--;
			j--;
		}
		if (j == 0)
			return k + 1;

		i += d[T[i - 1]];
	}
	return -1;
}

int buscar(char* texto, char* padrao) {

	cout << "> buscando " << padrao << " nos dados recebidos..." << endl;

	int ocorr;

	ocorr = BMH(texto, strlen(texto), padrao, strlen(padrao));
	if (ocorr >= 0)

		cout << "> padrao foi encontrado em " << ocorr << "..." << endl;
	else

		cout << "> o padrao nao foi encontrado..." << endl;

	return ocorr;
}
