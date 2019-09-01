#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
 
#define SERVER "127.0.0.1"
#define BUFLEN 512 
#define PORT 8888
#define QUEUE_SIZE 92
#define MSG_SIZE 100
#define NUM_ROT 8  

typedef struct{
    int id, port, msg_in, msg_out;
    char ip[32];
}router;

typedef struct{
    int origin, dest, type;
    char content[MSG_SIZE];

}package;

typedef struct{
    int origin, dest, cost, adj[NUM_ROT]
}table;

int main() {
  int tab_rot[NUM_ROT][NUM_ROT];
  
  return 0;
}