/*
    === DESCRIPTION ===
    The client randomly chooses one of the elements from the server's
    array, and randomly decides to read or write to them with a 95:5 ratio.
*/

#include "service.h"
#include "timer.h"
#include <semaphore.h>

// === CONSTANTS ===
#define WRITE_PERCENTAGE 5 //The percentage of requests that are writes
#define READ_PERCENTAGE 95 //The percentage of requests that are reads
#define REQUESTS_TO_MAKE 1000

// === GLOBAL VARIABLES ===
int port; //The port used to connect to the server
int arraySize; //The size of the array held on the server
int* seed;
sem_t sem;

int WeightedCoinToss(int seedIndex)
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
    int upperBound = 100 / (100 - READ_PERCENTAGE);
    int weightedRand = rand_r(&seed[seedIndex]) % upperBound;
    if(weightedRand < (upperBound - 1)){ // minus 1 for zero indexing
        return 0;
    }
    return 1;
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
    int request = (int)args;
    struct sockaddr_in sock_var;
    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = port;
    sock_var.sin_family = AF_INET;

    int clientFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(clientFileDescriptor, (struct sockaddr*)&sock_var, sizeof(sock_var)) >= 0)
    {
        char element[16];
        sprintf(element, "%d", rand_r(&seed[request]) % arraySize);

        int willWrite = WeightedCoinToss(request);
        if(willWrite)
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
    }
    else
    {
        printf("socket creation failed\n");
    }
    close(clientFileDescriptor);
    sem_post(&sem);
    pthread_exit(NULL);
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

    seed = malloc(REQUESTS_TO_MAKE*sizeof(int));
    for (int i = 0; i < REQUESTS_TO_MAKE; i++)
    {
        seed[i] = i;
    }

    port = atoi(argv[1]);
    arraySize = atoi(argv[2]);

    int threadCount = atoi(argv[1]);
    pthread_t t[MAX_THREADS];
    sem_init(&sem, 0, MAX_THREADS);

    double start; double end;
    GET_TIME(start);

    int requestsMade;
    for(int i = 0; i <= MAX_THREADS; i++)
    {
        i = i % MAX_THREADS;
        sem_wait(&sem);
        pthread_create(&t[i], NULL, ClientAction, (void*)requestsMade);
        if(++requestsMade == REQUESTS_TO_MAKE) break;
    }
    
    for(int i = 0; i < MAX_THREADS; i++)
    {
        pthread_join(t[i], NULL);
    }
    GET_TIME(end);
    
    printf("EXECUTION TIME: %lf \t REQUESTS MADE: %d\n", (end - start), requestsMade);

    return 0;
}