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
#define REQUESTS_TO_MAKE 1000 //The amount of requests that the client will launch

// === GLOBAL VARIABLES ===
int port; //The port used to connect to the server
int arraySize; //The size of the array held on the server
int* seed;
int successfulRequests;

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
    char element[16];
    char stringToWrite[MAX_STRING_LENGTH];
    char serverResp[MAX_STRING_LENGTH];

    int request = (intptr_t)args;
    int upperBound = 100 / (100 - READ_PERCENTAGE);
    int weightedRand = rand_r(&seed[request]) % upperBound;
    int willWrite = (weightedRand < (upperBound - 1)) ? 0 : 1;
    struct sockaddr_in sock_var;

    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = port;
    sock_var.sin_family = AF_INET;

    int clientFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    int connected = connect(clientFileDescriptor, (struct sockaddr*)&sock_var, sizeof(sock_var));

    if(connected >= 0 && clientFileDescriptor >= 0)
    {
        successfulRequests++;
        snprintf(element, 16, "%d", rand_r(&seed[request]) % arraySize);


        if(willWrite) // Write string to server's array'
        {
            snprintf(stringToWrite, MAX_STRING_LENGTH, "%sString %s has been modified by a write request", element, element);
            int written = write(clientFileDescriptor, stringToWrite, MAX_STRING_LENGTH);

            if(written == -1)
            {
                perror("Client Write Error");
            }
        }
        else // Read string from server's array
        {
            write(clientFileDescriptor, element, 16); // Write element we want to read
            int itWasRead = read(clientFileDescriptor, serverResp, MAX_STRING_LENGTH);

            if(itWasRead == -1)
            {
                perror("Client Read Error");
            }
        }
    }
    else if(connected == -1)
    {
        perror("Client Connect Error");
    }
    else if(clientFileDescriptor == -1)
    {
        perror("Client Socket Error");
    }

    close(clientFileDescriptor);
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

    successfulRequests = 0;
    pthread_t t[MAX_THREADS];

    double start; double end;
    GET_TIME(start);

    int j;
    for(j = 0; j < REQUESTS_TO_MAKE;)
    {
        for(int i = 0; i <= MAX_THREADS; i++)
        {
            int created = pthread_create(&t[i], NULL, ClientAction, (void*)(intptr_t)j);

            if(created == -1)
            {
                perror("Client Create Error");
            }

            if(++j == REQUESTS_TO_MAKE) break;
        }
        
        for(int i = 0; i < MAX_THREADS; i++)
        {
            int joined = pthread_join(t[i], NULL);
            if(joined == -1)
            {
                perror("Client Join Error");
            }
        }
    }
    GET_TIME(end);
    
    printf("EXECUTION TIME: %lf \t SUCCESSFUL REQUESTS MADE: %d / %d\n", (end - start), successfulRequests, j);

    free(seed);
    return 0;
}