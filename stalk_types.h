#ifndef _STALK_TYPES_H
#define _STALK_TYPES_H

#define MSG_TX_LEN 20
#define MSG_RX_LEN 20

#include "list.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


struct cleanup_handle_args_s
{
    pthread_mutex_t * mutexToUnlock;
    void * dataToFree;
};

typedef struct cleanup_handle_args_s cleanup_args;

struct cleaner_args_s 
{
    pthread_t * thread1;
    pthread_t * thread2;
    pthread_t * thread3;
    pthread_t * thread4;
    pthread_mutex_t * sysLock;
    pthread_cond_t * sysFlag;
    pthread_cond_t * txFlag;
    pthread_cond_t * rxFlag;
};

struct user_arguements_s
{
    List * data;
    pthread_mutex_t * mutex;
    pthread_cond_t * mutexCond;
    FILE * filePtr;
    //Flag to singal worker if any 
    pthread_cond_t * sysFlag;
};

struct rx_argument_s {
    int socketId;
    List * rxData;
    pthread_mutex_t * rxMutex;
    pthread_cond_t * rxCond;
    pthread_cond_t * sysFlag;
};

struct tx_arguements_s {
    int socketId;
    List * dataTx;
    pthread_mutex_t * mutex;
    pthread_cond_t * mutexCond;
    struct sockaddr_in * peerCnx;
    pthread_cond_t * sysFlag;
};

void cleanup_handler(void * itemsToClean);

#endif //_STALK_TYPES_H