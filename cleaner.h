#ifndef _CLEANER_H
#define _CLEANER_H

#include "stalk_types.h"

void * cleaner_thread(void * argIn);
int Cleaner_Init(void * argsIn);
void Cleaner_Shutdown(void);

#endif //_CLEANER_H