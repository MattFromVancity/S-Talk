/*
user_output.c - User_Output module for stalk
Author - Matt Whitehead mbwhiteh@sfu.ca
*/
#include "user_output.h"

static pthread_t threadId = 0;
static int threadRt = 0;
static struct user_arguements_s outputArgs = {NULL,NULL,NULL,NULL,NULL};

void * stdo_thread(void * argsIn)
{        
    //If a message is avaliable and a mutex is avaliable then grab the data
    struct user_arguements_s * argsOut = (struct user_arguements_s *) argsIn;
    char * msgTx = NULL;
    char * formatMsg = NULL;
    char endCheck[4] = "\n!\n";
    int mutexErr = 0;

    //Init the cleanup handler 
    cleanup_args itemsToClean = {argsOut->mutex,msgTx};
    cleanup_args itemsToCleanV2 = {argsOut->mutex,formatMsg};

    while(mutexErr == 0)
    {
        mutexErr = pthread_mutex_lock(argsOut->mutex);
        {
            //Wait for messages to be received before printing
            mutexErr = pthread_cond_wait(argsOut->mutexCond,argsOut->mutex);
            msgTx = List_trim(argsOut->data);
        }
        mutexErr = pthread_mutex_unlock(argsOut->mutex);
        
        //Loop through the messages in the shared received messages buffer to obtain all messages
        while(msgTx != NULL)
        {
            //Extra byte for null terminator as sprintf includes the null character
            formatMsg = malloc((strlen(msgTx)+1)*sizeof(char));
            
            sprintf(formatMsg,"%s",msgTx);

            //Update the clean up handler and its arguements
            itemsToClean.dataToFree = msgTx;
            itemsToCleanV2.dataToFree = formatMsg;
            pthread_cleanup_push(cleanup_handler,&itemsToClean);
            pthread_cleanup_push(cleanup_handler,&itemsToCleanV2);

            //Note ** write is a cancellation point update clean up handlers before
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
            mutexErr = write(STDOUT_FILENO,formatMsg,strlen(formatMsg));
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);

            //If message sent is a "!\n" initiate the cleaner case if no file is piped
            if(msgTx[0] == '!' && msgTx[1] == '\n' && strlen(msgTx) == 2)
            {
                mutexErr = pthread_cond_signal(argsOut->sysFlag);
            }
            //If the file is piped then find the occurace of "\n!\n"
            if(strstr(formatMsg,endCheck) != NULL)
            {
                mutexErr = pthread_cond_signal(argsOut->sysFlag);
            }
            
            //Free the dynamic memory allocated by the receieve thread
            free(msgTx);
            free(formatMsg);
           
            //Any cancelation requets
            pthread_testcancel();
            
            //Grab another message off the buffer
            mutexErr = pthread_mutex_lock(argsOut->mutex);
            {
                msgTx = List_trim(argsOut->data);
            }
            mutexErr = pthread_mutex_unlock(argsOut->mutex);

            //Remove currnet loop addresses from the clean up handler
            pthread_cleanup_pop(0);
            pthread_cleanup_pop(0);
        }
        if(mutexErr != 0)
        {
            printf("ERROR: pthread_mutex error in %s\n",__FILE__);
            mutexErr = pthread_cond_signal(argsOut->sysFlag);
        }
    }
    pthread_exit(NULL);
}

pthread_t User_Output_Init(List * dataIn,pthread_mutex_t * mutexIn,\
    pthread_cond_t * mutexCondIn, FILE * streamIn,pthread_cond_t * sysFlagIn)
{
    //Fill in the thread arguements
    outputArgs.data = dataIn;
    outputArgs.mutex = mutexIn;
    outputArgs.mutexCond = mutexCondIn;
    outputArgs.filePtr = streamIn;
    outputArgs.sysFlag = sysFlagIn;

    threadRt = pthread_create(&threadId,NULL,stdo_thread,&outputArgs);
    return threadId;
}
void User_Output_Shutdown()
{
    if(pthread_join(threadId,NULL) != 0)
    {
        printf("ERROR: pthread_join in %s\n",__FILE__);
    }
}