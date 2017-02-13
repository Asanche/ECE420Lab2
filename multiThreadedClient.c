#include "multiThreadedCS.h"
#include <time.h>

#define WRITE_PERCENTAGE 5
#define READ_PERCENTAGE 95
#define THREAD_COUNT 1000

int port;
int arraySize;

int ReadOrWrite()
{
    /*
        === DESCRIPTION ===
        This function decides whether to read or write. It returns 
        a value of 0 if you should read, and 1 if you should write.
        It works by getting a random value from rand(), modding the
        resultant number by 100 to get a value from 1 to 100, then 
        adding the percentage with which we should read with (in this 
        case that is 95%). We then mod this value by 100 again. Essentially
        we get that the value of readOrWrite will only be between 95 and 100
        if rand() produces a value of 1-5 (5%). We simply use this fact to 
        say we should only write if readOrWrite is 96-100
    */
    int readOrWrite = (rand() % 100 + READ_PERCENTAGE) % 100;
    if(readOrWrite <= READ_PERCENTAGE){
        return 0;
    }
    else return 1;
}

void* ClientAction(void *args)
{
    struct sockaddr_in sock_var;
    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = port;
    sock_var.sin_family = AF_INET;

    int clientFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(clientFileDescriptor, (struct sockaddr*)&sock_var,sizeof(sock_var)) >= 0)
    {
        srand((int)args);

        printf("Connected to server %d \n", clientFileDescriptor);
        
        char element[16];
        sprintf(element, "%d", rand() % arraySize);
        printf("Element %s\n", element);
        
        write(clientFileDescriptor, element, 16);

        int readOrWrite = ReadOrWrite();
        if(readOrWrite)
        {
            char stringToWrite[MAX_STRING_LENGTH];
            sprintf(stringToWrite, "String %s has been modified by a write request", element);
            write(clientFileDescriptor, stringToWrite, MAX_STRING_LENGTH);
            printf("Wrote %s\n", stringToWrite);
        }
        else
        {
            write(clientFileDescriptor, "", MAX_STRING_LENGTH);
            char str_ser[MAX_STRING_LENGTH];
            read(clientFileDescriptor, str_ser, MAX_STRING_LENGTH);
            printf("String from Server: \"%s\"\n", str_ser);
        }

        close(clientFileDescriptor);
        pthread_exit(NULL);
    }
    else
    {
        printf("socket creation failed\n");
    }
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("please use the format ./client <port> <arraySize>");
        return 1;
    }

    port = argv[1];
    arraySize = argv[2];

    int threadCount = atoi(argv[1]);
    pthread_t t[THREAD_COUNT];

    for(int i = 0; i < THREAD_COUNT; i++)      //can support 20 clients at a time
    {
        
        pthread_create(&t[i], NULL, ClientAction, (void*)i);
    }
    
    for(int i = 0; i < THREAD_COUNT; i++)      //can support 20 clients at a time
    {
        pthread_join(t[i], NULL);
    }
    
    return 0;
}