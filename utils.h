// BIBLIOTECAS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

// ESTADOS
enum estados {
	iniciando,
	comunicando,
	finalizando,
	encerrado
};

// DEFINIÇÕES
#define PORT_DESTINY "5002"
#define PORT_ORIGIN "5002"
#define HOST "127.0.0.1"
#define BUFFER 512

// FUNC DEFINITIONS
int meuListen();
int meuSend(int sock, char *msg, int msgLen);
int meuRecv(int sock, char *buffer, int BUFSIZE);
int meuAccept(int sock, struct addrinfo *addr);
int meuConnect(int sock, struct addrinfo *addr);
int meuSocket();
