#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include <stdio_ext.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>


#define TIMEOUT_MAX 10    //tempo maximo timeout
#define TIMEOUT_MS 100
#define N_ROT 4       //número de roteadores
#define MSG_SIZE 100  //tamanho da mensagem
#define QUEUE_SIZE 100//tamanho da fila dos roteadores
#define TRUE 1
#define FALSE 0
#define INF 999999

typedef struct{       //estrutura do pacote
    int origin, dest, type;
    unsigned short ack;
    int num_pack;     //número do pacote
    char content[MSG_SIZE]; //conteudo da mensagem
}Package;

typedef struct router{//estrutura do roteador
     int id, port, waiting_ack;
     Package msg_in[QUEUE_SIZE], msg_out[QUEUE_SIZE]; //filas de entrada e saída
     char ip[32];
}Router;

typedef struct{       //Estrutura das tabelas de roteamentos
  int cost[N_ROT];    //custo
  int path[N_ROT];    //caminho
}Table;

typedef struct{       //estrutura matriz djikstra
    int visit, cost, prev;
}mat_dijkstra;