// For compiling and executing:
// cd Assignment2/Multithreaded-Server
// make
// ./server
// ./client

// For shared memory + semaphores in Cygwin:
// cygserver-config
// cygrunsrv -S cygserver
// net stop cygserver

#include "includes.h"

// Semaphore classes used in threads
sem_t *full[10];
sem_t *empty[10];
sem_t *mutex[10]; 

int main(int argc, char* argv[]){
    // Welcome text
    printf("\n");
    printLog("2803ICT Assignment 2: By Harry Rowe.");

    // SHARED MEMORY SETUP //

    // Initiate shared memory
    key_t ShmKey = ftok(".", 'a'); // Shared memory key using token of current path
    int ShmId = shmget(ShmKey, sizeof(struct Memory), IPC_CREAT | 0666); // Create shared memory
    struct Memory *ShmPtr; // Pointer to shared memory
    // Error handle for setting up shared memory
    if (ShmId < 0){
        printError("Cannot setup shared memory.");
    } else {
        printLog("Shared memory set.");
    }
    // Define pointer at shared memory using shmID
    ShmPtr = (struct Memory *) shmat(ShmId, NULL, 0);
    // Error handle for shared memory pointer
    if ((int) ShmPtr == -1){
        printError("Shared memory attachment error.");
    } else {
        printLog("Server has attached shared memory.");
    }
    // Set flags in shared memory and set slot stack
    struct Stack* s = stackInit(10);
    ShmPtr->clientFlag = 0;
    ShmPtr->activeQueries = 0;
    for (int i = 9; i >= 0; --i){
        ShmPtr->serverFlag[i] = 0;
        // Add slot elements onto stack
        stackPush(s, i);
    }
    // Set shared memory slot stack
    ShmPtr->slotAllocation = s;

    // Init semaphores
    char** sem1 = (char**) calloc(10, sizeof(char*));
    char** sem2 = (char**) calloc(10, sizeof(char*));
    char** sem3 = (char**) calloc(10, sizeof(char*));
    // Create unique semaphore name for each semaphore
    for (int i = 0; i < 10; i++){
        // Allocate space for each token
        sem1[i] = (char*) calloc(10, sizeof(char));
        sem2[i] = (char*) calloc(10, sizeof(char));
        sem3[i] = (char*) calloc(10, sizeof(char));
        // Assign each prefix
        char sem1Prefix[10] = "full";
        char sem2Prefix[10] = "empty";
        char sem3Prefix[10] = "mutex";
        // Append i to each
        char append = 'a'+i;
        strncat(sem1Prefix, &append, 1);
        strncat(sem2Prefix, &append, 1);
        strncat(sem3Prefix, &append, 1);
        // Assign to each semaphore char array
        sem1[i] = sem1Prefix;
        sem2[i] = sem2Prefix;
        sem3[i] = sem3Prefix;
        // Post when server has data to write
        full[i] = sem_open(sem1Prefix, O_CREAT, 0666, 0);
        // Server can only write to 1 slot at a time
        empty[i] = sem_open(sem2Prefix, O_CREAT, 0666, 1);
        // Mutal exclusion
        mutex[i] = sem_open(sem3Prefix, O_CREAT, 0666, 1);
        // Handle if empty val starts at 0
        int emptyVal, mutexVal;
        sem_getvalue(empty[i], &emptyVal);
        sem_getvalue(mutex[i], &mutexVal);
        while (emptyVal < 1){
            sem_post(empty[i]);
            emptyVal++;
        }
        while (mutexVal < 1){
            sem_post(mutex[i]);
            mutexVal++;
        }
    }

    // Print server setup
    printLog("Server setup. Awaiting client input...");

    // Server initiated - await client input
    while(1){
        
        // Await valid client input
        while (ShmPtr->clientFlag == 0);

        // Check if client is disconnecting
        if (ShmPtr->clientFlag == -1){
            printLog("Client wishes to disconnect.");
            break;
        }

        // Store number locally on server
        unsigned int n = ShmPtr->number;
        // Allocate slot number from stack
        int slotNumber = stackPop(ShmPtr->slotAllocation);
        // Increase active queries
        ShmPtr->activeQueries++;
        // Write back to client slot number
        ShmPtr->number = slotNumber;
        // Tell client that server has read data
        ShmPtr->clientFlag = 0;

        // Print recieved data & reserved slot
        printLog("New data recieved from client:");
        printf("\tClient: %u\n", n);
        printf("\tReserved Slot: %d\n\n", slotNumber+1);

        // Hold thread ID
        pthread_t threads[32];

        // // Rotate bit and 
        for (int i = 0; i < 32; i++){
            // Create data struct to send to thread
            struct ThreadData data = {n, slotNumber, ShmPtr};
            // Create thread
            if (pthread_create(&threads[i], NULL, &factorise, (void*) &data) != 0){
                printError("Failed to create server thread.");
            }
            // Sleep buffer between each factor
            usleep(500);
        }
    }
    
    // Wait for active threads to finish
    while (ShmPtr->activeQueries > 0);
    // Process disconnection
    printLog("Disconnecting...");

    // Close, unlink + free semaphores
    for (int i = 0; i < 10; i++){
        sem_close(full[i]);
        sem_close(empty[i]);
        sem_close(mutex[i]);
    }
    
    printLog("Detaching memory...");
    // Detach and remove/destroy shared memory
    shmdt((void*) ShmPtr);
    shmctl(ShmId, IPC_RMID, NULL);
    // Close server
    printLog("Server closing.");
    return 0;
}

// Producer function
void* factorise(void* args){
    // Retrieve data sent from main thread
    struct ThreadData *data = (struct ThreadData*) args;
    int slotNumber = data->slotNumber;
    int n = data->n;

    // Sleep buffer between each factor
    usleep(500);

    // Calculate factors

    // Wait for slot to be free
    sem_wait(empty[slotNumber]);
    sem_wait(mutex[slotNumber]);
    // Assign data to slot
    data->Shm->slot[slotNumber] = n;
    // Post to client
    sem_post(mutex[slotNumber]);
    sem_post(full[slotNumber]);

    // New data is availible for client read
    data->Shm->serverFlag[slotNumber] = 1;
    while (data->Shm->serverFlag[slotNumber] == 1);

    // Wait for slot to be free
    sem_wait(empty[slotNumber]);
    sem_wait(mutex[slotNumber]);
    // Thread finished
    data->Shm->slot[slotNumber] = -1;
    // New data is availible to read
    // Post to client
    sem_post(mutex[slotNumber]);
    sem_post(full[slotNumber]);

    // New data is availible for client read
    data->Shm->serverFlag[slotNumber] = 1;
    while (data->Shm->serverFlag[slotNumber] == 1);

}