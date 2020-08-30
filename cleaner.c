/*
cleaner.c - Cleaner module for stalk
Author - Matt Whitehead mbwhiteh@sfu.ca
*/
#include "cleaner.h"

static pthread_t threadId;

typedef struct cleaner_args_s clean_t;

//Cleaner thread to clean after signaled from core application threads
void * cleaner_thread(void * argIn)
{
    clean_t * cleanArgs = argIn;
    int termMsg = 0;
    int condMsg = 0;

    pthread_mutex_lock(cleanArgs->sysLock);
    //Wait until signaled by any other core application thread to shutdown 
    pthread_cond_wait(cleanArgs->sysFlag,cleanArgs->sysLock);
    {
        printf("Goodbye and thank you for using S-Talk!\n");
        //signaling each condition variable mutiple times to enusre all mutex are unlocked
        condMsg = pthread_cond_signal(cleanArgs->rxFlag);
        termMsg = pthread_cancel(*(cleanArgs->thread3));
        
        condMsg = pthread_cond_signal(cleanArgs->txFlag);
        termMsg = pthread_cancel(*(cleanArgs->thread1));

        termMsg = pthread_cancel(*(cleanArgs->thread2));
        
        condMsg = pthread_cond_signal(cleanArgs->rxFlag);
        termMsg = pthread_cancel(*(cleanArgs->thread4));
    }
    pthread_mutex_unlock(cleanArgs->sysLock);
    if(condMsg != 0)
        printf("ERROR: pthread_cond_signal returned %d\n",condMsg);
    
    if(termMsg != 0)
        printf("ERROR: pthread_cancel returned %d\n",termMsg);
    
    pthread_exit(NULL);
}

int Cleaner_Init(void * argsIn)
{
    return pthread_create(&threadId,NULL,cleaner_thread,argsIn);
}

void Cleaner_Shutdown(void)
{
    if(pthread_join(threadId,NULL) !=0)
        printf("ERROR: pthread_join in %s\n",__FILE__);
}
