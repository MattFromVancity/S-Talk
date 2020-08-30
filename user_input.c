/*
user_input.c - User_Input module for stalk
Author - Matt Whitehead mbwhiteh@sfu.ca
*/
#include "user_input.h"

static pthread_t threadId = 0;
static int threadRt = 0;
static struct user_arguements_s user_args = {NULL,NULL,NULL,NULL,NULL};

void * stdin_thread(void * argsIn)
{
    //Incoming pointer is a List * to the tx_message list
    struct user_arguements_s * args = (struct user_arguements_s *) argsIn;
    char * msgSend = NULL;
    size_t bytesRead = 0;
    char mutexErr = 0;
    //Init the cleanup handler 
    cleanup_args itemsToClean = {args->mutex,msgSend};

    while(mutexErr == 0)
    {
        msgSend = malloc(sizeof(char)*MSG_TX_LEN);
        memset(msgSend,0,MSG_TX_LEN);
        //Update the cleanup handler 
        itemsToClean.dataToFree = msgSend;      
        pthread_cleanup_push(cleanup_handler,&itemsToClean);

        //Read message in from the user
        bytesRead = read(STDIN_FILENO,msgSend,(size_t)MSG_TX_LEN - 1);

        //Insert an string termination character
        msgSend[bytesRead] = '\0'; 

        if(msgSend[0] == '!' && msgSend[1] == '\n')
        {
            //Singnal worker thread for shutdown
            mutexErr = pthread_cond_signal(args->sysFlag);
        }
        
        if(bytesRead > 0 && msgSend != NULL)
        {
            mutexErr = pthread_mutex_lock(args->mutex);
            {
                //Add an item to the shared buffer and signal the transmission thread
                List_prepend(args->data,msgSend);
                mutexErr = pthread_cond_signal(args->mutexCond);
            }

            mutexErr = pthread_mutex_unlock(args->mutex);
        }

        //EOF from file ensure all data is sent!
        if(bytesRead <= 0)
        {
            mutexErr = pthread_mutex_lock(args->mutex);
            {
                //Once all data is sent, make the cleaner do some work
                if(List_count(args->data) == 0)
                {
                    mutexErr = pthread_cond_signal(args->sysFlag);
                }
            }
            mutexErr = pthread_mutex_unlock(args->mutex);

        }
        //Pop off the current loops clean up arguments
        pthread_cleanup_pop(0);
    }
    //Must be matched with a PUSH as per POSIX
    pthread_exit(NULL);
}

pthread_t User_Input_Init(List * data,pthread_mutex_t * mutexIn,pthread_cond_t * mutexCondIn,FILE * stream,\
    pthread_cond_t* sysFlagIn)
{
    //Fill in the user args to send to thread
    user_args.data = data;
    user_args.mutex = mutexIn;
    user_args.mutexCond = mutexCondIn;
    user_args.filePtr = stream;
    user_args.sysFlag = sysFlagIn;
    
    threadRt = pthread_create(&threadId,NULL,stdin_thread,&user_args);
    return threadId;
}

void User_Input_Shutdown(void)
{
    if(pthread_join(threadId,NULL) != 0)
    {
        printf("ERROR: pthread_join in %s\n",__FILE__);
    }
}   