/*
stalk.c - Bidirectional UDP Chat Application
Author - Matt Whitehead mbwhiteh@sfu.ca
*/

#include "receive.h"
#include "transmit.h"
#include "user_input.h"
#include "user_output.h"
#include "cleaner.h"
#include "stalk_types.h"

pthread_mutex_t mutex_tx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_rx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_term = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_mutex_tx = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_mutex_rx = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_mutex_term = PTHREAD_COND_INITIALIZER;

int main(int argc,char * argv[])
{
    //Initilization of S-talk
    if(argc < 4)
    {
        printf("Args Err: Specify 4 or more arguements");
        exit(-1);
    }
    
    //Welcome message
    printf("Welcome to S-Talk!\n");

    //Host
    struct sockaddr_in hostSin;
    memset(&hostSin,0,sizeof(struct sockaddr_in));
    hostSin.sin_family = AF_INET;
    hostSin.sin_port = htons(atoi(argv[1]));
    hostSin.sin_addr.s_addr = INADDR_ANY;

    //Init and bind socket the local socket
    int sId = socket(PF_INET,SOCK_DGRAM,getprotobyname("udp")->p_proto);
    if(bind(sId,(struct sockaddr*) &hostSin, sizeof(hostSin)) == - 1)
    {
        printf("Socket bind error %d\n",errno);
        exit(-1);
    }

    //Shared thread buffer lists for passing tx and rx buffers
    List * txList = List_create();
    List * rxList = List_create(); 

    //Thread initialization
    pthread_t tx = Transmit_Init(sId,txList,&mutex_tx,&cond_mutex_tx,argv[3],argv[2],&cond_mutex_term);
    pthread_t user_in = User_Input_Init(txList,&mutex_tx,&cond_mutex_tx,stdin,&cond_mutex_term);
    pthread_t user_out = User_Output_Init(rxList,&mutex_rx,&cond_mutex_rx,stdout,&cond_mutex_term);
    pthread_t rx = Receive_Init(sId,rxList,&mutex_rx,&cond_mutex_rx,&cond_mutex_term);
    
    //DO WORK
    struct cleaner_args_s clean = {&tx,&user_in,&user_out,&rx,\
        &mutex_term,&cond_mutex_term,&cond_mutex_tx,&cond_mutex_rx};

    if(Cleaner_Init(&clean) != 0)
    {
        printf("ERROR: Cleaner_Init failed in %s\n",__FILE__);
        exit(-1);
    }

    //Shutting down remaning modules 
    User_Input_Shutdown();
    Transmit_Shutdown();
    Receive_Shutdown();
    User_Output_Shutdown();

    Cleaner_Shutdown();

    //Free any remaining items in the list
    List_free(txList,free);
    List_free(rxList,free);

    if(close(sId) != 0)
        printf("ERROR: close(%d) returned with error code %d\n",sId,errno);

    int mxError = 0;
    int condError = 0;
    
    mxError = pthread_mutex_destroy(&mutex_tx);
    mxError = pthread_mutex_destroy(&mutex_rx);
    mxError = pthread_mutex_destroy(&mutex_term);
    condError = pthread_cond_destroy(&cond_mutex_tx);
    condError = pthread_cond_destroy(&cond_mutex_rx);
    condError = pthread_cond_destroy(&cond_mutex_term);

    if(mxError != 0)
        printf("ERROR: pthread_mutex_destroy returned %d\n",mxError);

    if(condError != 0)
        printf("ERROR: pthread_cond_destroy returned %d\n",condError);

    return 0;
}
