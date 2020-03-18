// BIBLIOTECAS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

// DEFINIÇÕES
#define PORT "5002"
#define BUFFER 512

// STRUCTS
struct addrinfo *addr;

struct sockaddr_storage recv_addr;
socklen_t addr_len = sizeof recv_addr;

// ESTADOS
enum estados {
	iniciando,
	comunicando,
	finalizando,
	encerrado
};

// DECLARAÇÕES DE FUNÇÕES
int meuListen();
int meuAccept(int sock, struct addrinfo *addr);
int meuSend(int sock, char *msg, int msgLen);
int meuRecv(int sock, char *buffer, int BUFSIZE);
int meuSocket();

// FUNÇÃO MAIN
int main(int argc, char const *argv[]){
    int estado_atual = iniciando;
    int servSock, clntSock;

    while (estado_atual != encerrado){

        switch (estado_atual){
            case iniciando:
                servSock = meuSocket();
                estado_atual = comunicando;
                break;
            case comunicando:
                estado_atual = finalizando;
                break;
            case finalizando:
                estado_atual = encerrado;
                close(servSock);
                break;
            default:
                break;
        }
    }

    return EXIT_SUCCESS;
}

// DEMAIS FUNÇÕES
int meuListen(){
    puts("Iniciando escuta...");
    return 0;
}

int meuAccept(int sock, struct addrinfo *addr) {
    int sendMsg, recvMsg;
    const char *msgSend = "SYNACK";
    const char *msgTest = "TESTADO";
    char buffer[BUFFER];

    puts("Esperando conexão...");

    recvMsg = recvfrom(sock, buffer, BUFFER, 0, (struct sockaddr *)&recv_addr, &addr_len);
    printf("Recebi ");
    fputs(buffer, stdout);
    puts("\n");

    if (recvMsg < 0) {
        perror("Erro ao receber mensagem.\n");
        return EXIT_FAILURE;
    }

    if (strcmp(buffer, "SYN") == 0) {
        sendMsg = sendto(sock, msgSend, sizeof(&msgSend), 0, (struct sockaddr *)&recv_addr, addr_len);

        if (sendMsg < 0) {
            perror("Erro ao enviar mensagem.\n");
            return EXIT_FAILURE;
        }
        printf("Enviei %s\n", msgSend);
        puts("Conexão aceita.");
    }

    printf("\nEnviei %s\n", msgTest);
    meuSend(sock, msgTest, sizeof(msgTest));
    meuRecv(sock, buffer, sizeof(buffer));
    printf("Recebi ");
    fputs(buffer, stdout);
    puts("\n");

    return 0;
}

int meuSend(int sock, char *msg, int msgLen){
    int sendMsg = sendto(sock, msg, msgLen, 0, (struct sockaddr *)&recv_addr, addr_len);

    return sendMsg;
}
int meuRecv(int sock, char *buffer, int BUFSIZE){
    int recvMsg = recvfrom(sock, buffer, BUFSIZE, 0, (struct sockaddr *)&recv_addr, &addr_len);

    return recvMsg;
}

int meuSocket() {
	struct addrinfo addrCriteria;
	memset(&addrCriteria, 0, sizeof(addrCriteria));
	addrCriteria.ai_family = AF_UNSPEC;
	addrCriteria.ai_flags = AI_PASSIVE;
	addrCriteria.ai_socktype = SOCK_DGRAM;
	addrCriteria.ai_protocol = IPPROTO_UDP;

	struct addrinfo *servAddr;

	int rtnVal = getaddrinfo(NULL, PORT, &addrCriteria, &servAddr);

	if (rtnVal != 0) {
        perror("getaddrinfo() falhou");
        exit(EXIT_FAILURE);
	}

	int sock = -1;

	for (addr = servAddr; addr != NULL; addr = addr->ai_next) {
        sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

        if (sock < 0)
            continue;

        if (bind(sock, addr->ai_addr, addr->ai_addrlen) == 0) {
            meuAccept(sock, addr);
            break;
        }

        meuListen();
        close(sock);
        sock = -1;
	}
	freeaddrinfo(servAddr);
	return sock;
}
