#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>

#define BUFLEN 512  //Max length of buffer
#define N_ROT 6
#define MSG_SIZE 100

typedef struct router{
     int id, port, msg_in, msg_out;
     char ip[32];
}Router;

typedef struct{
    int origin, dest, type;
    char content[MSG_SIZE];
}Package;

typedef struct{  //Estrutura das tabelas de roteamentos
  int cost[N_ROT]; //custo
  int path[N_ROT]; //caminho
}Table;
