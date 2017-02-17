/*
    === DESCRIPTION ===
    The server simply listens for client requests that are either
    read or write, and either reads or writes to an element
    in the array of strings that it holds specified by the 
    client
*/
#include "service.h"
#define WRITE_SUCcESS "String sucessfully written to array."

// === GLOBAL VARIABLES ===
char** theArray; // The array of strings held in memory for the client to read or write to
pthread_rwlock_t   rwl; // The mutex that prevents race conditions b/w threads
pthread_mutex_t fileMutex;
int count;
double times[MAX_THREADS];

void WriteFile(){
    FILE* fp;
    fp = fopen("results/rwl_results.txt", "a");

    if (fp == NULL)
    {
        fp = fopen("results/rwl_results.txt", "w+");
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

    char* stringToWriteTemp; // For strtol as it can't use a buffer >.>
    char stringToWrite[MAX_STRING_LENGTH];
    char clientString[MAX_STRING_LENGTH];
    char readString[MAX_STRING_LENGTH];
    
    int itWasRead =  read(clientFileDescriptor, clientString, MAX_STRING_LENGTH);

    if(itWasRead == -1)
    {
        perror("Server Read Error");
    }

    int element = strtol(clientString, &stringToWriteTemp, 10);
    strncpy(stringToWrite, stringToWriteTemp, MAX_STRING_LENGTH);

    if(strlen(stringToWrite) == 0) // Read desired string
    {
        GET_TIME(start);
        pthread_rwlock_rdlock(&rwl);
        strncpy(readString, theArray[element], MAX_STRING_LENGTH); // Copy to buffer is safer than directly reading
        printf("R \t ELEMENT: %d \t STRING: %s\n", element, readString);
        pthread_rwlock_unlock(&rwl);
        GET_TIME(end);
        int written = write(clientFileDescriptor, readString, MAX_STRING_LENGTH); // Write back for client
        
        if(written == -1)
        {
            perror("Server Write Error 1");
        }
    }
    else
    {
        GET_TIME(start);
        pthread_rwlock_wrlock(&rwl);
        strncpy(theArray[element], stringToWrite, MAX_STRING_LENGTH);
        printf("W \t ELEMENT: %d \t STRING: %s\n", element, stringToWrite);
        pthread_rwlock_unlock(&rwl);
        GET_TIME(end);
        int written = write(clientFileDescriptor, stringToWrite, MAX_STRING_LENGTH); // Write back for client
        
        if(written == -1)
        {
            perror("Server Write Error 2");
        }
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
*/
    if(argc != 3)
    {
        printf("please use the format ./server <port> <arraySize>");
        return 1;
    }

    count = 0;
    int port = atoi(argv[1]);
    int arraySize = atoi(argv[2]);
    int clientFileDescriptor;
    int serverFileDescriptor = socket(AF_INET,SOCK_STREAM, 0);
    struct sockaddr_in sock_var;
    pthread_t t[MAX_THREADS];

    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = port;
    sock_var.sin_family = AF_INET;

    pthread_rwlock_init(&rwl, NULL);
    pthread_mutex_init(&fileMutex, NULL);
    
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
    
    int bound = bind(serverFileDescriptor, (struct sockaddr*)&sock_var,sizeof(sock_var));


    if(bound >= 0 && serverFileDescriptor >= 0)
    {
        int heard = listen(serverFileDescriptor, 2000); 

        if (heard == -1) 
        {
            perror("Server Listening Error");
        }

        while(1)//loop infinitely
        {
            for(int i = 0; i < MAX_THREADS; i++)
            {
                clientFileDescriptor = accept(serverFileDescriptor, NULL, NULL);
                if(clientFileDescriptor >= 0)
                {
                    int created = pthread_create(&t[i], NULL, ServerDecide, (void *)(intptr_t)clientFileDescriptor);
                    if(created == -1)
                    {
                        perror("Server Create Error");
                    }
                }
                else if(clientFileDescriptor == -1)
                {
                    perror("Server Accept Error");
                }
            }

            for(int i = 0; i < MAX_THREADS; i++)
            {
                int joined = pthread_join(t[i], NULL);
                if(joined == -1)
                {
                    perror("Server Join Error");
                }
            }

            WriteFile();
        }
    }
    else if(bound == -1)
    {
        perror("Server Bind Error");
    }
    else if(serverFileDescriptor == -1)
    {
        perror("Server Socket Error");
    }
    
    return 0;
}