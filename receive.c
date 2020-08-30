/*
receive.c - Receive module for stalk
Author - Matt Whitehead mbwhiteh@sfu.ca
*/
#include "receive.h"

static pthread_t tId = 0;
static int threadRt = 0;
static struct rx_argument_s rxThreadArgs = {0,NULL,NULL,NULL,NULL};

//Acutal Threads
void * rx_thread(void * argsIn)
{
    struct rx_argument_s * rxArgs = (struct rx_argument_s *) argsIn;
    int bytesRx = 0;
    int mutexErr = 0;
    char * msgRx = NULL;

    //Push thread clean up handler to stack and init args
    cleanup_args cleanTheseItems = {rxArgs->rxMutex, msgRx};

    //Template Socket for recvfrom call
    struct sockaddr_in rxSin;
    memset(&rxSin,0,sizeof(struct sockaddr_in));
    rxSin.sin_family = AF_INET;
    rxSin.sin_addr.s_addr = INADDR_ANY;
    socklen_t rxSinSize = 0;
    
    while(mutexErr == 0)
    {
        msgRx = malloc(sizeof(char)*MSG_RX_LEN);

        //Saving some data to 
        cleanTheseItems.dataToFree = msgRx;
        pthread_cleanup_push(cleanup_handler,&cleanTheseItems);

        bytesRx = recvfrom(rxArgs->socketId,msgRx,MSG_RX_LEN,MSG_WAITALL,(struct sockaddr *) &rxSin, &rxSinSize);

        if(bytesRx == 0)
        {
            printf("!! Peer Disconnected\n");
        }else if(bytesRx < 0)
        {
            printf("ERROR: Couldn't receive data\n");
            mutexErr = pthread_cond_signal(rxArgs->sysFlag);
        }
        

        //Data has been recieved, add it to the list and allocate new memory for next message else use areadly allocated memory
        if(msgRx != NULL)
        {
            //Add the message to the Rx_Buffer list if it is not being accessed
            mutexErr = pthread_mutex_lock(rxArgs->rxMutex);
            {
                List_prepend(rxArgs->rxData,msgRx);
                mutexErr = pthread_cond_signal(rxArgs->rxCond);
            }
            mutexErr = pthread_mutex_unlock(rxArgs->rxMutex);
        }
        //Test for a cancelation request after the mutex is unlocked
        pthread_testcancel();
        pthread_cleanup_pop(0);
    }
    if(mutexErr != 0)
    {
        printf("ERROR: Mutex error\n");
        mutexErr = pthread_cond_signal(rxArgs->sysFlag);
    }
    //Must match the push call as per POSIX
    pthread_exit(NULL);
}

//Creates a pthread to receive UDP datagrams
pthread_t Receive_Init(int socketId,List * rxData,pthread_mutex_t * rxMutex,pthread_cond_t * rxCond,pthread_cond_t * sysFlagIn)
{
    rxThreadArgs.socketId = socketId;
    rxThreadArgs.rxData = rxData;
    rxThreadArgs.rxMutex = rxMutex;
    rxThreadArgs.rxCond = rxCond;
    rxThreadArgs.sysFlag = sysFlagIn;
    threadRt = pthread_create(&tId,NULL,rx_thread,&rxThreadArgs);

    return tId;
}

//Shutdown and clean up the receive thread
void Receive_Shutdown(void)
{
    if(pthread_join(tId,NULL) != 0)
        printf("ERROR: pthread_join(Receive_Shutdown)\n");
}