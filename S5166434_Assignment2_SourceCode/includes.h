#ifndef INCLUDES_H
#define INCLUDES_H

// Include libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

// Include files
#include "prototypes.h"

// Macros
#define BACKLOG 5 // Max number of requests recieved by clients
#define SIZE 1024 // Max size for char arrays

// Shared memory
struct Memory{
    // Variables
    unsigned int number;
    int slot[10];
    int activeQueries;
    // Flags
    int serverFlag[10];
    int clientFlag;
    // Slot stack
    int slotAllocation[10];
    int slotAllocationCounter;
    // Progress measures
    float progress[10];
};

// Data package to sent to shared memory
struct ThreadData{
    unsigned int n;
    int slotNumber;
};

#endif