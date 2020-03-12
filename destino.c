// Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

// Maquina de Estados: para gerênciar o funcionamento da aplicação
enum estados {
  iniciando,
  comunicando,
  finalizando,
  encerrado
}

// Tamanho do buffer
static const int BUFSIZE = 512;

// Criação da Função meuListen();
int meuListen() {
  puts("-> Iniciando escuta;\n");
  return 0;
}

// Criação do Socket
int meuSocket() {
  struct addrinfo addrCriteria;
  memset(&addrCriteria, 0, sizeof(addrCriteria));
  addrCriteria.ai_family = AF_UNSPEC;
  addrCriteria.ai_flags = AI_PASSIVE;
  addrCriteria.ai_socktype = SOCK_DGRAM;
  addrCriteria.ai_protocol = IPPROTO_UDP;
  
  struct addrinfo *servAddr;
  int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
  
  if (rtnVal != 0) {
    perror("getaddrinfo() falhou\n");
    exit(EXIT_FAILURE);
  }
  
  int servSock = -1;
  
  for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next) {
    servSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    
    if (servSock < 0)
      continue;
    
    // Função Bind()
    if (bind(servSOck, addr->ai_addr, addr->ai_addrlen) == 0) 
      break;
    
    // Função meuListen()
    if (meuListen() == 0)
      break;
      
    close(servSock);
    servSock = -1;
  }
  
  freeaddrinfo(servAddr);
  
  return servSock;
}

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    perror("Execução correta: ./destino <porta>\n");
    return EXIT_FAILURE;
  }
  
  int estado_atual = iniciando;
  int servSock, clntSock;
  
  while (estado_atual != encerrado) {
    switch (estado_atual) {
      case inicando:
        break;
      case comunicando:
        break;
      case finalizando:
        break;
      default: 
        break;
    }
  }
  
  return EXIT_SUCCESS;
}





























