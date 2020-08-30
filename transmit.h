#ifndef _TRANSMIT_H
#define _TRANSMIT_H

#include "stalk_types.h"

pthread_t Transmit_Init(int socketIdIn,List * dataIn, pthread_mutex_t * mutexIn, pthread_cond_t * mutexCondIn,\
    char * txPortIn,char * txAddrIn,pthread_cond_t * sysFlagIn);
void Transmit_Shutdown(void);

#endif 