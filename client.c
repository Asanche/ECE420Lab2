/*
    === DESCRIPTION ===
    The client randomly chooses one of the elements from the server's
    array, and randomly decides to read or write to them with a 95:5 ratio.
*/

#include "service.h"
#include "timer.h"

// === CONSTANTS ===
#define WRITE_PERCENTAGE 5 //The percentage of requests that are writes
#define READ_PERCENTAGE 95 //The percentage of requests that are reads

// === GLOBAL VARIABLES ===
int port; //The port used to connect to the server
int arraySize; //The size of the array held on the server

int ReadOrWrite()
{/*
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
{/*
    === DESCRIPTION === 
    This is the thread function for the client. It connects
    to the server using a socket. It then randomly chooses
    an element to read or write to. It uses ReadOrWrite()
    to determine whether it should read or write.
    
    === PARAMETERS ===
    void* args - this expects the thread number, and is used
        to seed the random number generator
*/
    struct sockaddr_in sock_var;
    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = port;
    sock_var.sin_family = AF_INET;

    int clientFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(clientFileDescriptor, (struct sockaddr*)&sock_var,sizeof(sock_var)) >= 0)
    {
        srand((int)args);

        char element[16];
        sprintf(element, "%d", rand() % arraySize);

        int readOrWrite = ReadOrWrite();
        if(readOrWrite)
        {
            char stringToWrite[MAX_STRING_LENGTH];
            sprintf(stringToWrite, "%sString %s has been modified by a write request", element, element);
            write(clientFileDescriptor, stringToWrite, MAX_STRING_LENGTH);
        }
        else
        {
            write(clientFileDescriptor, element, 16);
            char str_ser[MAX_STRING_LENGTH];
            read(clientFileDescriptor, str_ser, MAX_STRING_LENGTH);
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
{/*
    === DESCRIPTION === 
    This is the main client function. It launches 1000
    threads that randomly choose an element in the array
    to read (95%) or write to (5%). If it reads it, it prints
    the value.

    expects an input from the command linee like so:
    ./client <port> <arraySize>
    
    === PARAMETERS ===
    int argc - number of command line variables input.
    char* argv[] - input variables from the command line

    === COMMAND LINE ARGUMENTS ===
    argv[1] - Port to connect to (ususally 3000)
    argv[2] - The size of the server's array of strings
    
    === OUTPUTS ===
    1 - failure
    0 - ran to completion
*/
    if(argc != 3)
    {
        printf("please use the format ./client <port> <arraySize>");
        return 1;
    }

    port = atoi(argv[1]);
    arraySize = atoi(argv[2]);

    int threadCount = atoi(argv[1]);
    pthread_t t[THREAD_COUNT];

    double start; double end;
    GET_TIME(start);
    for(int i = 0; i < THREAD_COUNT; i++)      //can support 20 clients at a time
    {
        
        pthread_create(&t[i], NULL, ClientAction, (void*)i);
    }
    
    for(int i = 0; i < THREAD_COUNT; i++)      //can support 20 clients at a time
    {
        pthread_join(t[i], NULL);
    }
    GET_TIME(end);
    
    printf("EXECUTION TIME: %lf\n", (end - start));

    return 0;
}