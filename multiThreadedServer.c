#include "multiThreadedCS.h"

char theArray[ARRAY_SIZE][MAX_STRING_LENGTH];
pthread_mutex_t mutex;

char* ReadString(int element)
{
    pthread_mutex_lock(&mutex); 
    char* readString = theArray[element];
    printf("Read \"%s\"\n", readString);
    pthread_mutex_unlock(&mutex);
    return readString;
}

void WriteString(int element, char* string)
{
    pthread_mutex_lock(&mutex); 
    strcpy(theArray[element], string);
    printf("Wrote %s\n", string);
    pthread_mutex_unlock(&mutex);
}

void* ServerDecide(void *args)
{
    /* 
    === DESCRIPTION ===
        This function is responsible for deciding whether a request
        is a read or a write request. It reads three strings from the
        clientFileDescriptor into three strings each on a new line 
        representing the desired action (R/W), the element the with 
        which to read or write and, if it is a write, the string they 
        wish to write to that element
        
    === PARAMETERS ===
        void* args - this is the input file from the client
    */

    int clientFileDescriptor = (int)args;
    printf("Connected to client %d\n", clientFileDescriptor);
    char readOrWrite[1];
    char arrayElement[16];

    read(clientFileDescriptor, readOrWrite, 1);
    read(clientFileDescriptor, arrayElement, 16);

    printf("Yo.... %s", readOrWrite);
    if(strcmp(readOrWrite,"R"))
    {
        printf("Reading element %s\n", arrayElement);
        write(clientFileDescriptor, ReadString(atoi(arrayElement)), MAX_STRING_LENGTH);
    }
    // else if(strcmp(readOrWrite,"W"))
    // {
    //     char stringToWrite[MAX_STRING_LENGTH];
    //     read(clientFileDescriptor, stringToWrite, MAX_STRING_LENGTH);
    //     printf("Writing \"%s\" to element %s\n", stringToWrite, arrayElement);
    //     WriteString(atoi(arrayElement), stringToWrite);
    // }

    close(clientFileDescriptor);
}

int main()
{
    //Initialize the strings appropriately
    for(int i = 0; i < ARRAY_SIZE; i++)
    {
        sprintf(theArray[i], "String %d: the initial value", i);
    }

    struct sockaddr_in sock_var;
    int serverFileDescriptor = socket(AF_INET,SOCK_STREAM, 0);
    int clientFileDescriptor;
    int i;

    pthread_mutex_init(&mutex, NULL);

    pthread_t t[20];

    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = 3000;
    sock_var.sin_family = AF_INET;
    
    if(bind(serverFileDescriptor, (struct sockaddr*)&sock_var,sizeof(sock_var)) >= 0)
    {
        printf("Socket has been created\n");
        listen(serverFileDescriptor,2000); 
        while(1)//loop infinitely
        {
            for(i = 0; i < 20; i++)
            {
                clientFileDescriptor = accept(serverFileDescriptor, NULL, NULL);
                pthread_create(&t[i], NULL, ServerDecide, (void *)clientFileDescriptor);
            }
        }
        close(serverFileDescriptor);
    }
    else
    {
        printf("Socket creation failed\n");
    }
    return 0;
}