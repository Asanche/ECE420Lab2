/*
    === DESCRIPTION ===
    The server simply listens for client requests that are either
    read or write, and either reads or writes to an element
    in the array of strings that it holds specified by the 
    client
*/
#include "service.h"

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
    
    int itWasRead =  read(clientFileDescriptor, clientString, MAX_STRING_LENGTH);

    if(itWasRead == -1)
    {
        perror("Server Read Error");
    }

    int element = strtol(clientString, &stringToWrite, 10);

    if(strlen(stringToWrite) == 0)
    {
        readLock(&rwl); 
        char* readString = theArray[element];
        rwUnlock(&rwl);
        int written = write(clientFileDescriptor, readString, MAX_STRING_LENGTH);
        
        if(written == -1)
        {
            perror("Server Write Error");
        }
    }
    else
    {
        writeLock(&rwl); 
        strncpy(theArray[element], stringToWrite, MAX_STRING_LENGTH);
        rwUnlock(&rwl);
    }

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
    int canReuseAddr = setsockopt(serverFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

    int clientFileDescriptor;

    rwlockInit(&rwl);

    pthread_t t[MAX_THREADS];

    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = port;
    sock_var.sin_family = AF_INET;
    
    int bound = bind(serverFileDescriptor, (struct sockaddr*)&sock_var,sizeof(sock_var));

    if(bound >= 0)
    {
        int heard = listen(serverFileDescriptor, 2000); 

        if (heard == -1) {
            perror("Server Listening Error");
        }

        while(1)//loop infinitely
        {
            for(int i = 0; i < MAX_THREADS; i++)
            {
                clientFileDescriptor = accept(serverFileDescriptor, NULL, NULL);
                if(clientFileDescriptor >= 0)
                {
                    pthread_create(&t[i], NULL, ServerDecide, (void *)(intptr_t)clientFileDescriptor);
                }
                else if(clientFileDescriptor == -1)
                {
                    perror("Server Accept Error");
                }
            }
        }
        close(serverFileDescriptor);
    }
    else if(bound == -1)
    {
        perror("Server Bind Error");
    }
    else if(serverFileDescriptor == -1)
    {
        perror("Server Socket Error");
    }
    else if(canReuseAddr == -1)
    {
        perror("Server Socket Option SO_REUSEADDR Error");
    }

    freeArray(arraySize);
    return 0;
}