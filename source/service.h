#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include "timer.h"

// === CONSTANTS ===
#define MAX_STRING_LENGTH 1024 //The maximum length of string tha can e held in the server's list
#define MAX_THREADS 1000 //The amount of threads used
