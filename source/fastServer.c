/*
    === DESCRIPTION ===
    The server simply listens for client requests that are either
    read or write, and either reads or writes to an element
    in the array of strings that it holds specified by the 
    client
*/
#include "service.h"
#define THREAD_COUNT 1000

typedef struct {
    int readers;
    int writer;
    pthread_cond_t readers_proceed;
    pthread_cond_t writer_proceed;
    int pending_writers;
    pthread_mutex_t read_write_lock;
} rwlock_t;

// === GLOBAL VARIABLES ===
char** theArray; // The array of strings held in memory for the client to read or write to
rwlock_t rwl; // The mutex that prevents race conditions b/w threads

void freeArray(int arraySize)
{
    for(int i = 0; i < arraySize; i++)
    {
        free(theArray[i]);
    }
    free(theArray);
}

void rwlockInit (rwlock_t* rwl) {
    rwl -> readers = rwl -> writer = rwl -> pending_writers = 0;
    pthread_mutex_init(&(rwl -> read_write_lock), NULL);
    pthread_cond_init(&(rwl -> readers_proceed), NULL);
    pthread_cond_init(&(rwl -> writer_proceed), NULL);
}

void readLock(rwlock_t* rwl) {
    /* if there is a write lock or pending writers, perform
    condition wait, else increment count of readers and grant
    read lock */
    pthread_mutex_lock(&(rwl -> read_write_lock));

    while ((rwl -> pending_writers > 0) || (rwl -> writer > 0))
    {
        pthread_cond_wait(&(rwl -> readers_proceed), &(rwl -> read_write_lock));
    }

    rwl -> readers++;
    pthread_mutex_unlock(&(rwl -> read_write_lock));
}

void writeLock(rwlock_t* rwl) {
    /* if there are readers or writers, increment pending
    writers count and wait. On being woken, decrement pending
    writers count and increment writer count */
    pthread_mutex_lock(&(rwl -> read_write_lock));

    while ((rwl -> writer > 0) || (rwl -> readers > 0)) {
        rwl -> pending_writers++;
        pthread_cond_wait(&(rwl -> writer_proceed), &(rwl -> read_write_lock));
        rwl -> pending_writers--;
    }

    rwl -> writer++;
    pthread_mutex_unlock(&(rwl -> read_write_lock));
}

void rwUnlock(rwlock_t* rwl) {
    /* if there is a write lock then unlock, else if there
    are read locks, decrement count of read locks. If the count
    is 0 and there is a pending writer, let it through, else if
    there are pending readers, let them all go through */
    pthread_mutex_lock(&(rwl -> read_write_lock));

    if (rwl -> writer > 0)
    {
        rwl -> writer = 0;
    }
    else if (rwl -> readers > 0)
    {
        rwl -> readers--;
    }

    pthread_mutex_unlock(&(rwl -> read_write_lock));

    if ((rwl -> readers == 0) && (rwl -> pending_writers > 0))
    {
        pthread_cond_signal(&(rwl -> writer_proceed));
    }
    else if (rwl -> readers > 0)
    {
        pthread_cond_broadcast(&(rwl -> readers_proceed));
    }
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
    readLock(&rwl); 
    char* readString = theArray[element];
    rwUnlock(&rwl);
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
    writeLock(&rwl); 
    strcpy(theArray[element], string);
    rwUnlock(&rwl);
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

    char* stringToWrite;
    char clientString[MAX_STRING_LENGTH];
    
    read(clientFileDescriptor, clientString, MAX_STRING_LENGTH);

    int element = strtol(clientString, &stringToWrite, 10);

    if(strlen(stringToWrite) == 0)
    {
        write(clientFileDescriptor, ReadString(element), MAX_STRING_LENGTH);
    }
    else
    {
        WriteString(element, stringToWrite);
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

    rwlockInit(&rwl);

    pthread_t t[THREAD_COUNT];

    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = port;
    sock_var.sin_family = AF_INET;
    
    if(bind(serverFileDescriptor, (struct sockaddr*)&sock_var,sizeof(sock_var)) >= 0)
    {
        listen(serverFileDescriptor,2000); 
        while(1)//loop infinitely
        {
            for(int i = 0; i < THREAD_COUNT; i++)
            {
                clientFileDescriptor = accept(serverFileDescriptor, NULL, NULL);
                pthread_create(&t[i], NULL, ServerDecide, (void *)(intptr_t)clientFileDescriptor);
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