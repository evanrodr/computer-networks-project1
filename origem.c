#include "utils.h"

struct addrinfo *addr;

struct sockaddr_storage recv_addr;
socklen_t addr_len = sizeof recv_addr;

int main(int argc, char const *argv[]) {
    int estado_atual = iniciando;
    int sock;

    while (estado_atual != encerrado) {
        switch (estado_atual) {
            case iniciando:
                sock = meuSocket();
                estado_atual = comunicando;
                break;
            case comunicando:
                estado_atual = finalizando;
                break;
            case finalizando:
                estado_atual = encerrado;
                // close(servSock);
                // close(clntSock);
                break;
            default:
                break;
            }
    }

    return EXIT_SUCCESS;
}

int meuConnect(int sock, struct addrinfo *addr) {
    int sendMsg, recvMsg;
    const char *msgSend = "SYN";
    const char *msgTest = "TESTE";
    char buffer[BUFFER];
    
    puts("Pedindo conexão...");
    sleep(2);
    printf("Enviei %s\n", msgSend);
    sendMsg = sendto(sock, msgSend, sizeof(&msgSend), 0, addr->ai_addr, addr->ai_addrlen);
    sleep(2);
    if(sendMsg < 0) {
        perror("Erro ao enviar mensagem\n");
        return EXIT_FAILURE;
    }

    recvMsg = recvfrom(sock, buffer, BUFFER, 0, (struct sockaddr *)&recv_addr, &addr_len);
    if(recvMsg < 0) {
        perror("Erro ao receber mensagem\n");
        return EXIT_FAILURE;
    }

    if (strcmp(buffer, "SYNACK") == 0) {
        printf("\nRecebi ");
        fputs(buffer, stdout);
        puts("\nConexão estabelecida.\n");
    }

    sleep(2);
    printf("Enviei %s\n", msgTest);
    meuSend(sock, (char *)msgTest, sizeof(msgTest));
    sleep(2);
    meuRecv(sock, buffer, sizeof(buffer));
    printf("Recebi ");
    fputs(buffer, stdout);
    puts("\n");
    
    return 0;
}

int meuSend(int sock, char *msg, int msgLen) {
    int sendMsg = sendto(sock, msg, msgLen, 0, addr->ai_addr, addr->ai_addrlen);
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
    addrCriteria.ai_socktype = SOCK_DGRAM;
    
    struct addrinfo *servAddr;
    
    int rtnVal = getaddrinfo(HOST, PORT_ORIGIN, &addrCriteria, &servAddr);
    if (rtnVal != 0){
        perror("getaddrinfo() falhou\n");
        exit(EXIT_FAILURE);
    }
    
    int sock = -1;
    for (addr = servAddr; addr != NULL; addr = addr->ai_next){

        sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sock < 0)
            continue;
        
        if (meuConnect(sock, addr) == 0){
            
            break;
        }
        
        close(sock);
        sock = -1;
    }
    freeaddrinfo(servAddr);
    return sock;
}
