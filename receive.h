#ifndef _RECEIVE_H_
#define _RECEIVE_H_

#include "stalk_types.h"

pthread_t Receive_Init(int socketId,List * rxData,pthread_mutex_t * rxMutex,\
    pthread_cond_t * rxCond,pthread_cond_t * sysFlagIn);
    
void Receive_Shutdown(void);


#endif 