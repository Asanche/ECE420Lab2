/*
    === DESCRIPTION ===
    The server simply listens for client requests that are either
    read or write, and either reads or writes to an element
    in the array of strings that it holds specified by the 
    client
*/
#include "../multiThreadedCS.h"

// === GLOBAL VARIABLES ===
char** theArray; // The array of strings held in memory for the client to read or write to
pthread_mutex_t mutex; // The mutex that prevents race conditions b/w threads

void freeArray(int arraySize)
{
    for(int i = 0; i < arraySize; i++)
    {
        free(theArray[i]);
    }
    free(theArray);
}

char* ReadString(int element)
{/*
    === DESCRIPTION ===
    This is the read function. It uses a mutex to ensure no other reads or writes
    occurr simultaneously. It simply returns the value read at a specified element
    in the array

    === PARAMETERS ===
    int element - the element of the array to read
*/
    pthread_mutex_lock(&mutex); 
    char* readString = theArray[element];
    pthread_mutex_unlock(&mutex);
    return readString;
}

void WriteString(int element, char* string)
{/*
    === DESCRIPTION ===
    This is the write function. It uses a mutex to ensure that no other writes
    or reads occur simultaneously.

    === PARAMETERS ===
    int element - the element of the array to write to
    char* string - the string to write to that element
*/
    pthread_mutex_lock(&mutex); 
    strcpy(theArray[element], string);
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
    char stringToWrite[MAX_STRING_LENGTH];
    char arrayElement[16];

    read(clientFileDescriptor, arrayElement, 16);
    read(clientFileDescriptor, stringToWrite, MAX_STRING_LENGTH);

    if(strlen(stringToWrite) == 0)
    {
        write(clientFileDescriptor, ReadString(atoi(arrayElement)), MAX_STRING_LENGTH);
    }
    else
    {
        WriteString(atoi(arrayElement), stringToWrite);
    }

    close(clientFileDescriptor);
}

int main(int argc, char* argv[])
{/*
    === DESCRIPTION ===
    This is the main function for the server. It listens for clients
    trying to connect, and connects with them on one of 20 threads.
    It then reads which element they are trying to read or write to,
    then reads again, and checks the length of the string. If it 
    has a length of 0, it is a read, otherwise it writes whatever
    it just read to that element

    expects an input from the command linee like so:
    ./server <port> <arraySize>
    
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
        printf("please use the format ./server <port> <arraySize>");
        return 1;
    }

    int port = atoi(argv[1]);
    int arraySize = atoi(argv[2]);

    //Malloc the array
    theArray = (char**)malloc(sizeof(char*) * arraySize);
    for(int i = 0; i < arraySize; i++)
    {
        theArray[i] = (char*)malloc(sizeof(char) * MAX_STRING_LENGTH);
    }

    //Initialize the strings appropriately
    for(int i = 0; i < arraySize; i++)
    {
        sprintf(theArray[i], "String %d: the initial value", i);
    }

    struct sockaddr_in sock_var;
    int serverFileDescriptor = socket(AF_INET,SOCK_STREAM, 0);
    int clientFileDescriptor;
    int i;

    pthread_mutex_init(&mutex, NULL);

    pthread_t t[TRHEAD_COUNT];

    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = port;
    sock_var.sin_family = AF_INET;
    
    if(bind(serverFileDescriptor, (struct sockaddr*)&sock_var,sizeof(sock_var)) >= 0)
    {
        listen(serverFileDescriptor,2000); 
        while(1) //loop infinitely
        {
            for(i = 0; i < TRHEAD_COUNT; i++)
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

    freeArray(arraySize);
    return 0;
}