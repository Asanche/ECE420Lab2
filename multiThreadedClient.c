#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define WRITE_PERCENTAGE = 5
#define READ_PERCENTAGE = 95

int ReadOrWrite()
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
        return 0;
    }
    else return 1;
}

int main()
{
    struct sockaddr_in sock_var;
    int clientFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    int threadCount;
    char str_ser[20];

    sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_var.sin_port = 3000;
    sock_var.sin_family = AF_INET;

    if(connect(clientFileDescriptor, (struct sockaddr*)&sock_var,sizeof(sock_var)) >= 0)
    {
        printf("Connected to server %dn", clientFileDescriptor);
        printf("Enter a number of threads to randomly read or write to the server");
        scanf("%d", threadCount);
        write(clientFileDescriptor, str_clnt, 20);
        read(clientFileDescriptor, str_ser, 20);
        printf("String from Server: %s", str_ser);
        close(clientFileDescriptor);
    }
    else
    {
        printf("socket creation failed");
    }

    return 0;
}