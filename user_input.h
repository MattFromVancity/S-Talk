#ifndef _USER_INPUT_H
#define _USER_INPUT_H

#include "stalk_types.h"

pthread_t User_Input_Init(List * data,pthread_mutex_t * mutexIn,pthread_cond_t * mutexCondIn,FILE * stream,\
    pthread_cond_t* sysFlagIn);
void User_Input_Shutdown(void);

#endif //_USER_INPUT_H