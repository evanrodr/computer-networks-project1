#include "utils.h"

// STRUCTS
struct addrinfo *addr;
struct sockaddr_storage recv_addr;

socklen_t addr_len = sizeof recv_addr;

// FUNÇÃO MAIN
int main(int argc, char const *argv[]) {
    int estado_atual = iniciando;
    int servSock, clntSock;

    while (estado_atual != encerrado){
        switch (estado_atual) {
            case iniciando:
                notificarEstados(1);
                servSock = meuSocket();
                estado_atual = comunicando;
                break;
            case comunicando:
                estado_atual = finalizando;
                break;
            case finalizando:
                estado_atual = encerrado;
                close(servSock);
                notificarEstados(4);
                break;
            default:
                break;
        }
    }

    return EXIT_SUCCESS;
}

void notificarEstados(int notif) {
    sleep(1);
    switch (notif) {
        case 1:
            puts("> Criando socket...");
            break;
        case 2:
            puts("> Esperando conexão...");
            break;
        case 3: 
            puts("\n> Conexão aceita.\n");
            break;
        case 4: 
            puts("\n> Conexão encerrada.");
            break;
        default:
            break;
    }
}

// DEMAIS FUNÇÕES
int meuListen() {
    puts("> Iniciando escuta...");
    return 0;
}

int meuAccept(int sock, struct addrinfo *addr) {
    int sendMsg, recvMsg;
    const char *msgSend = "SYNACK";
    const char *msgTest = "TESTADO";
    char buffer[BUFFER];

    notificarEstados(2);

    recvMsg = recvfrom(sock, buffer, BUFFER, 0, (struct sockaddr *)&recv_addr, &addr_len);
    printf("\n> Recebi ");
    fputs(buffer, stdout);

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
        printf("\n> Enviei %s\n", msgSend);
        notificarEstados(3);
    }

    meuRecv(sock, buffer, sizeof(buffer));
    sleep(1);
    printf("> Recebi ");
    fputs(buffer, stdout);
    printf("\n> Enviei %s\n", msgTest);
    meuSend(sock, (char *)msgTest, sizeof(msgTest));
    
    return 0;
}

int meuSend(int sock, char *msg, int msgLen) {
    int sendMsg = sendto(sock, msg, msgLen, 0, (struct sockaddr *)&recv_addr, addr_len);
    return sendMsg;
}

int meuRecv(int sock, char *buffer, int BUFSIZE) {
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

	int rtnVal = getaddrinfo(NULL, PORT_DESTINY, &addrCriteria, &servAddr);

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
            meuListen();
            meuAccept(sock, addr);
            break;
        }

        close(sock);
        sock = -1;
	}
	freeaddrinfo(servAddr);
	return sock;
}
