#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>

#define SERVER "127.0.0.1"
#define BUFLEN 512  //Max length of buffer

// typedef struct{
//     int id, port, msg_in, msg_out;
//     char ip[32];
// }router;

// typedef struct{
//     int origin, dest, type;
//     char content[MSG_SIZE];

// }package;

// typedef struct{
//     int origin, dest, cost, path[NUM_ROT];
// }table;