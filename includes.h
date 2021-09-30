#ifndef INCLUDES_H
#define INCLUDES_H

// Include libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <limits.h>

// Include files
#include "prototypes.h"

// Macros
#define BACKLOG 5 // Max number of requests recieved by clients
#define SIZE 1024 // Max size for char arrays

// Shared memory

struct Memory{
    // Client exit status
    int clientStatus;
    // Variables
    unsigned int number;
    unsigned int slot[10];
    // Client Flags
    int clientFlag;
    int serverFlag[10];
};

#endif