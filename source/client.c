/*
    === DESCRIPTION ===
    The client randomly chooses one of the elements from the server's
    array, and randomly decides to read or write to them with a 95:5 ratio.
*/

#include "service.h"

// === CONSTANTS ===
#define READ_PERCENTAGE 95 //The percentage of requests that are reads
#define MAX_FILE_DESCRIPTORS 256

// === GLOBAL VARIABLES ===
int port; //The port used to connect to the server
int arraySize; //The size of the array held on the server
int* seed;


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
    char stringToWrite[MAX_STRING_LENGTH];
    char serverResp[MAX_STRING_LENGTH];

    //Error values
    int written;
    int itWasRead;

    int request = (intptr_t)args;
    int element = rand_r(&seed[request]) % arraySize;
    int upperBound = 100 / (100 - READ_PERCENTAGE);
    int weightedRand = rand_r(&seed[request]) % upperBound;
    int willWrite = (weightedRand < (upperBound - 1)) ? 0 : 1;
    struct sockaddr_in sock_var;

    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = port;
    sock_var.sin_family = AF_INET;
    
    int clientFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    int connected = connect(clientFileDescriptor, (struct sockaddr*)&sock_var, sizeof(sock_var));

    if(willWrite) // Write string to server's array'
    {
        snprintf(stringToWrite, MAX_STRING_LENGTH, "%dString %d has been modified by a write request", element, element);
    }
    else // Read string from server's array
    {
        snprintf(stringToWrite, MAX_STRING_LENGTH, "%d", element); // Write element we want to read
    }
    
    if(connected >= 0 && clientFileDescriptor >= 0)
    {
        written = write(clientFileDescriptor, stringToWrite, MAX_STRING_LENGTH);
        itWasRead = read(clientFileDescriptor, serverResp, MAX_STRING_LENGTH);
        
        printf("SERVER RETURNED: \t %s\n", serverResp);

        close(clientFileDescriptor);
    }
    else if(connected == -1)
    {
        perror("Client Connect Error");
    }
    else if(clientFileDescriptor == -1)
    {
        perror("Client Socket Error");
    }

    if(written == -1) // Was the value written properly?
    {
        perror("Client Write Error");
    }

    if(itWasRead == -1) // Was the value read properly?
    {
        perror("Client Read Error");
    }

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

    seed = malloc(MAX_THREADS*sizeof(int));
    for (int i = 0; i < MAX_THREADS; i++)
    {
        seed[i] = i;
    }

    port = atoi(argv[1]);
    arraySize = atoi(argv[2]);

    pthread_t t[MAX_THREADS];

    int i;
    for(i = 0; i < MAX_THREADS; i++)
    {
        int created = pthread_create(&t[i], NULL, ClientAction, (void*)(intptr_t)i);

        if(created == -1)
        {
            perror("Client Create Error");
        }
    }
    
    for(int j = 0; j < MAX_THREADS; j++)
    {
        int joined = pthread_join(t[j], NULL);
        if(joined == -1)
        {
            perror("Client Join Error");
        }
    }
    
    free(seed);
    return 0;
}