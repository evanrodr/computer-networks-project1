#include "utils.h"

// SOCKETS DO DESTINO E ORIGEM
int sockDestino, sockOrigem;

// INICIALIZAÇÕES
struct addrinfo *destinoAddr;
struct addrinfo *origemAddr;
struct sockaddr_storage destinoRecv_addr;
struct sockaddr_storage origemRecv_addr;
socklen_t destinoAddr_len = sizeof destinoRecv_addr;
socklen_t origemAddr_len = sizeof origemRecv_addr;

// FUNÇÃO MAIN
int main(int argc, char const *argv[]) {
    int estado_atual = iniciando;
    
    // MAQUINA DE ESTADOS
    while (estado_atual != encerrado) {
        switch (estado_atual){
            case iniciando:
                notificarEstados(1);
                sockDestino = socketDestino(PORT_ORIGIN);
                notificarEstados(2);
                sockOrigem = socketOrigem(HOST, PORT_DESTINY);
                estado_atual = comunicando;
                break;
            case comunicando:
                comutador();
                estado_atual = finalizando;
                break;
            case finalizando:
                estado_atual = encerrado;
                close(sockDestino);
                close(sockOrigem);
                break;
            default:
                break;
        }
    }
    notificarEstados(4);
    return EXIT_SUCCESS;
}

// NOTIFICADOR
void notificarEstados(int notif) {
    sleep(1);
    switch (notif) {
        case 1:
            puts("> Criando socket do destino.");
            break;
        case 2: 
            puts("> Criando socket da origem.");
            break;
        case 3:
            puts("\n> Aguardando destino e origem...");
            break;
        case 4:
            puts("\n> Conexão encerrada.");
            break;
        default:
            break;
    }
}

// FUNÇÃO COMUTADOR
void comutador() {
    int sendMsg,recvMsg;
    char buffer[BUFFER];
    int i, cont = 0;

    notificarEstados(3);
    
    while(cont < 2){
        memset(buffer, 0, sizeof(buffer));
        recvMsg = recvfrom(sockDestino, buffer, BUFFER, 0, (struct sockaddr*)&destinoRecv_addr, &destinoAddr_len);
        sendMsg = sendto(sockOrigem, buffer, BUFFER, 0, origemAddr->ai_addr, origemAddr->ai_addrlen);
        
        memset(buffer, 0, sizeof(buffer));
        recvMsg = recvfrom(sockOrigem, buffer, BUFFER, 0, (struct sockaddr*)&origemRecv_addr, &origemAddr_len);
        sendMsg = sendto(sockDestino, buffer, BUFFER, 0, (struct sockaddr*)&destinoRecv_addr, destinoAddr_len);

        cont++;
    }
}

// CRIAÇÃO DO SOCKET DE ORIGEM
int socketOrigem(const char *ip, const char *porta) {
    struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria));
    addrCriteria.ai_family = AF_UNSPEC;
    addrCriteria.ai_socktype = SOCK_DGRAM;
    addrCriteria.ai_protocol = IPPROTO_UDP;

    struct addrinfo *servAddr;
    int rtnVal = getaddrinfo(ip, porta, &addrCriteria, &servAddr);
    if (rtnVal != 0){
        {
            perror("getaddrinfo() falhou\n");
            exit(EXIT_FAILURE);
        }
    }

    int sock = -1;
    for (origemAddr = servAddr; origemAddr != NULL; origemAddr = origemAddr->ai_next){
        sock = socket(origemAddr->ai_family, origemAddr->ai_socktype, origemAddr->ai_protocol);
        if (sock < 0)
            continue;
        break;
        close(sock);
        sock = -1;
    }
    freeaddrinfo(servAddr);
    return sock;
}

// CRIAÇÃO DO SOCKET DE DESTINO
int socketDestino(const char *porta) {
    struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria)); 
    addrCriteria.ai_family = AF_UNSPEC;             
    addrCriteria.ai_flags = AI_PASSIVE;             
    addrCriteria.ai_socktype = SOCK_DGRAM;         
    addrCriteria.ai_protocol = IPPROTO_UDP;        

    struct addrinfo *servAddr;
    int rtnVal = getaddrinfo(NULL, porta, &addrCriteria, &servAddr);
    if (rtnVal != 0){
        perror("getaddrinfo() falhou\n");
        exit(EXIT_FAILURE);
    }

    int servSock = -1;
    for (destinoAddr = servAddr; destinoAddr != NULL; destinoAddr = destinoAddr->ai_next) {
        servSock = socket(destinoAddr->ai_family, destinoAddr->ai_socktype, destinoAddr->ai_protocol);
        if (servSock < 0)
            continue;

        if ((bind(servSock, destinoAddr->ai_addr, destinoAddr->ai_addrlen) == 0)){
            break;
        }

        close(servSock);
        servSock = -1;
    }

    freeaddrinfo(servAddr);

    return servSock;
}
