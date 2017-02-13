#include "multiThreadedCS.h"
#include <time.h>

#define WRITE_PERCENTAGE 5
#define READ_PERCENTAGE 95

char* ReadOrWrite()
{
    /*
        === DESCRIPTION ===
        This function decides whether to read or write. It returns 
        a value of 0 if you should write, and 1 if you should write.
        It works by getting a random value from rand(), modding the
        resultant number by 100 to get a value from 1 to 100, then 
        adding the percentage with which we should read with (in this 
        case that is 95%). We then mod this value by 100 again. Essentially
        we get that the value of readOrWrite will only be between 95 and 100
        if rand() produces a value of 1-5 (5%). We simply use this fact to 
        say we should only write if readOrWrite is 96-100
    */
    int readOrWrite =  (rand() % 100 + READ_PERCENTAGE) % 100;
    if(readOrWrite <= READ_PERCENTAGE){
        return "R";
    }
    else return "W";
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    struct sockaddr_in sock_var;
    int clientFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(argc != 2)
    {
        printf("You did not input a valid # of theads! Please run this as ./client <threads>");
    }

    int threadCount = atoi(argv[1]);

    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = 3000;
    sock_var.sin_family = AF_INET;

    if(connect(clientFileDescriptor, (struct sockaddr*)&sock_var,sizeof(sock_var)) >= 0)
    {
        printf("Connected to server %d \n", clientFileDescriptor);
        char* readOrWrite = "R";//ReadOrWrite();
        write(clientFileDescriptor, readOrWrite, 1);
        printf("Wrote %s \n", readOrWrite);

        char element[16];
        sprintf(element, "%d", rand() % ARRAY_SIZE);
        printf("Wrote %s\n", element);
        
        write(clientFileDescriptor, element, 16);

        char str_ser[MAX_STRING_LENGTH];
        read(clientFileDescriptor, str_ser, MAX_STRING_LENGTH);
        printf("String from Server: \"%s\"\n", str_ser);
        close(clientFileDescriptor);
    }
    else
    {
        printf("socket creation failed\n");
    }

    return 0;
}