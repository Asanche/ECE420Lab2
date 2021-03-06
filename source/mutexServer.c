/*
    === DESCRIPTION ===
    The server simply listens for client requests that are either
    read or write, and either reads or writes to an element
    in the array of strings that it holds specified by the 
    client. ALL accesses to theArray are controlled with a single mutex
    whether they are reads or writes.
*/
#include "service.h"

// === GLOBAL VARIABLES ===
char** theArray; // The array of strings held in memory for the client to read or write to
pthread_mutex_t mutex; // The mutex that prevents race conditions b/w threads
pthread_mutex_t fileMutex; // controls access to the count variable below
int count; // Helps threads wrote to times
double times[MAX_THREADS]; // Execution times to be writte (one per thread)

void WriteFile(){
    FILE* fp;
    fp = fopen("results/mutex_results.txt", "a");

    if (fp == NULL)
    {
        fp = fopen("results/mutex_results.txt", "w+");
    }

    for(int i = 0; i < MAX_THREADS; i++)
    {
        fprintf(fp, "%lf\n", times[i]);
    }
    count = 0;
    fclose(fp);
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
    int clientFileDescriptor = (intptr_t)args;
    double start; double end;

    char* stringToWrite;
    char clientString[MAX_STRING_LENGTH];
    char readString[MAX_STRING_LENGTH];
    
    read(clientFileDescriptor, clientString, MAX_STRING_LENGTH);

    int element = strtol(clientString, &stringToWrite, 10);

    if(strlen(stringToWrite) == 0)
    {
        GET_TIME(start);
        pthread_mutex_lock(&mutex);
        strncpy(readString, theArray[element], MAX_STRING_LENGTH);
        printf("R \t ELEMENT: %d \t STRING: %s\n", element, readString);
        pthread_mutex_unlock(&mutex);
        GET_TIME(end);
        write(clientFileDescriptor, readString, MAX_STRING_LENGTH);
    }
    else
    {
        GET_TIME(start);
        pthread_mutex_lock(&mutex);
        snprintf(theArray[element], MAX_STRING_LENGTH, "%s", stringToWrite);
        printf("W \t ELEMENT: %d \t STRING: %s\n", element, stringToWrite);
        pthread_mutex_unlock(&mutex);
        GET_TIME(end);
    }

    pthread_mutex_lock(&fileMutex);
    int index = count++;
    pthread_mutex_unlock(&fileMutex);
    times[index] = (end - start);

    close(clientFileDescriptor);
    pthread_exit(NULL);
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

    //Check CL argument length
    if(argc != 3)
    {
        printf("please use the format ./server <port> <arraySize>");
        return 1;
    }

    //Get CL arguments
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
        snprintf(theArray[i], MAX_STRING_LENGTH, "String %d: the initial value", i);
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&fileMutex, NULL);

    pthread_t t[MAX_THREADS];

    struct sockaddr_in sock_var;
    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = port;
    sock_var.sin_family = AF_INET;
    
    int clientFileDescriptor;
    int serverFileDescriptor = socket(AF_INET,SOCK_STREAM, 0);
    if(bind(serverFileDescriptor, (struct sockaddr*)&sock_var,sizeof(sock_var)) >= 0)
    {
        //printf("Socket has been created\n");
        listen(serverFileDescriptor,2000); 
        while(1) //loop infinitely
        {
            for(int i = 0; i < MAX_THREADS; i++)
            {
                clientFileDescriptor = accept(serverFileDescriptor, NULL, NULL);
                pthread_create(&t[i], NULL, ServerDecide, (void*)(intptr_t)clientFileDescriptor);
            }

            for(int i = 0; i < MAX_THREADS; i++)
            {
                pthread_join(t[i], NULL);
            }
            WriteFile();
        }
        close(serverFileDescriptor);
    }
    else
    {
        printf("Socket creation failed\n");
    }

    return 0;
}