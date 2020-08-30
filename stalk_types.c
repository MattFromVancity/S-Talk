/*
stalk_types.c - Worker functions for the stalk application
Author - Matt Whitehead mbwhiteh@sfu.ca
*/
#include "stalk_types.h"

void cleanup_handler(void * itemsToClean)
{
    cleanup_args * cleanItems = itemsToClean;
    pthread_mutex_unlock(cleanItems->mutexToUnlock);
    
    if(cleanItems->dataToFree != NULL)
    {
        free(cleanItems->dataToFree);
    }
}