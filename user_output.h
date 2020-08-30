#ifndef _USER_OUTPUT_H
#define _USER_OUTPUT_H

#include "stalk_types.h" 

//Init the thread for user output
pthread_t User_Output_Init(List * dataIn,pthread_mutex_t * mutexIn,\
    pthread_cond_t * mutexCondIn, FILE * streamIn,pthread_cond_t * sysFlagIn);

//Hidden call to pthread_join
void User_Output_Shutdown();


#endif //_USER_OUTPUT_H