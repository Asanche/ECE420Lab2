#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

#define MAX_STRING_LENGTH = 128

char** theArray;
int arraySize = 100;


char* ReadString(int element)
{
    //mutex lock
    char* readString = theArray[element];
    //mutex unlock
    return readString;
}

void WriteString(int element, char* string)
{
    //mutex lock
    theArray[element] = string;
    //mutex unlock
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
    char[1] readOrWrite;
    char[16] arrayElement;

    read(clientFileDescriptor, readOrWrite, 1);
    read(clientFileDescriptor, arrayElement, 16);

    if(readOrWrite == 'R')
    {
        write(clientFileDescriptor, ReadString(atoi(arrayElement)), 20);
        printf("echoing back to client");
        
    }
    else if(readOrWrite == 'W')
    {
        char[MAX_STRING_LENGTH] stringToWrite;
        read(clientFileDescriptor, readOrWrite, 1);
        WriteString(stringToWrite);
    }

    close(clientFileDescriptor);
    // printf("reading from client: %s", str);
    // write(clientFileDescriptor, str, 20);
    // printf("echoing back to client");
    // close(clientFileDescriptor);
}


int main()
{
    //Malloc and initialize the strings appropriately
    theArray = (char**)malloc(sizeof(char*) * arraySize);
    for(int i = 0; i < arraySize; i++)
    {
        theArray[i] = malloc(sizeof(char) * MAX_STRING_LENGTH);
        sprintf(theArray[i], "String %d: the initial value", i)
    }

    struct sockaddr_in sock_var;
    int serverFileDescriptor = socket(AF_INET,SOCK_STREAM, 0);
    int clientFileDescriptor;
    int i;
    pthread_t t[20];

    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = 3000;
    sock_var.sin_family = AF_INET;
    
    if(bind(serverFileDescriptor, (struct sockaddr*)&sock_var,sizeof(sock_var)) >= 0)
    {
        printf("nsocket has been created");
        listen(serverFileDescriptor,2000); 
        while(1)        //loop infinity
        {
            for(i = 0; i < 1000; i++)      //can support 20 clients at a time
            {
                clientFileDescriptor=accept(serverFileDescriptor, NULL, NULL);
                printf("nConnected to client %dn", clientFileDescriptor);
                pthread_create(&t, NULL, ServerDecide, (void *)clientFileDescriptor);
            }
        }
        close(serverFileDescriptor);
    }
    else
    {
        printf("nsocket creation failed");
    }
    return 0;
}