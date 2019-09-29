#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>

#define BUFLEN 512    //tamanho máximo do buffer
#define N_ROT 6       //número de roteadores
#define MSG_SIZE 100  //tamanho da mensagem

typedef struct{       //estrutura do pacote
    int origin, dest, type;
    char content[MSG_SIZE];
}Package;

typedef struct router{//estrutura do roteador
     int id, port;
     Package msg_in, msg_out;
     char ip[32];
}Router;

typedef struct{       //Estrutura das tabelas de roteamentos
  int cost[N_ROT];    //custo
  int path[N_ROT];    //caminho
}Table;

typedef struct{       //estrutura matriz djikstra
    int visit, cost, prev;
}mat_djikstra;