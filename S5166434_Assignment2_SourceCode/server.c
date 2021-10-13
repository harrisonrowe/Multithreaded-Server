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

// Pointer to shared memory
struct Memory *ShmPtr; 

int main(int argc, char* argv[]){
    // Welcome text
    printf("\n");
    printLog("2803ICT Assignment 2: By Harry Rowe.");

    // SHARED MEMORY SETUP //

    // Initiate shared memory
    key_t ShmKey = ftok(".", 'a'); // Shared memory key using token of current path
    int ShmId = shmget(ShmKey, sizeof(struct Memory), IPC_CREAT | 0666); // Create shared memory
    
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
    // Set deafult values and flags in shared memory
    ShmPtr->number = 0;
    ShmPtr->clientFlag = 0;
    ShmPtr->activeQueries = 0;
    ShmPtr->slotAllocationCounter = -1;
    for (int i = 9; i >= 0; --i){
        ShmPtr->progress[i] = 0.0;
        ShmPtr->serverFlag[i] = 0;
        ShmPtr->slot[i] = 0;
        // Add slot elements onto slot allocation stack
        ShmPtr->slotAllocation[++ShmPtr->slotAllocationCounter] = i;
    }

    // LOAD SEMAPHORES //

    // Init semaphores - create unique sem name
    char** sem1 = (char**) calloc(10, sizeof(char*));
    char** sem2 = (char**) calloc(10, sizeof(char*));
    // Create unique semaphore name for each semaphore
    for (int i = 0; i < 10; i++){
        // Allocate space for each token
        sem1[i] = (char*) calloc(10, sizeof(char));
        sem2[i] = (char*) calloc(10, sizeof(char));
        // Assign each prefix
        char sem1Prefix[10] = "full";
        char sem2Prefix[10] = "empty";
        // Append i to each
        char append = 'a'+i;
        strncat(sem1Prefix, &append, 1);
        strncat(sem2Prefix, &append, 1);
        // Assign to each semaphore char array
        sem1[i] = sem1Prefix;
        sem2[i] = sem2Prefix;
        // Post when server has data to write
        full[i] = sem_open(sem1Prefix, O_CREAT, 0666, 0);
        // Server can only write to 1 slot at a time
        empty[i] = sem_open(sem2Prefix, O_CREAT, 0666, 1);
        // Handle if empty val starts at 0
        int emptyVal;
        sem_getvalue(empty[i], &emptyVal);
        while (emptyVal < 1){
            sem_post(empty[i]);
            emptyVal++;
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

        // Check if test mode is activated
        if (n == 0){
            // Test mode requested - setup threads
            printLog("Test mode activated.");
            // Hold thread ID
            pthread_t threads[30];
            void* ret;
            // Create set of threads
            for (int i = 0; i < 3; i++){
                for (int j = 0; j < 10; j++){
                    // Create data to send
                    struct ThreadData data = {j, 0};
                    if (pthread_create(&threads[(i*10)+j], NULL, &testmode, (void*) &data) != 0){
                        printError("Failed to create server test thread.");
                    }
                    // Sleep buffer between each thread - can be used if threads are too fast
                    usleep(10);
                }
            }
            // Join threads
            for (int i = 0; i < 30; i++){
                if (pthread_join(threads[i], &ret) != 0){
                    printError("Failed to join server thread");
                }
            }
            printLog("Test mode finished.");
        } else {
            // Allocate slot number from stack
            int slotNumber = ShmPtr->slotAllocation[ShmPtr->slotAllocationCounter--];
            // Increase active queries
            ShmPtr->activeQueries++;
            // Write back to client slot number
            ShmPtr->number = slotNumber;
            // Reset progress
            ShmPtr->progress[slotNumber] = 0.0;

            // Print recieved data & reserved slot
            printLog("New data recieved from client:");
            printf("\tClient: %u\n", n);
            printf("\tReserved Slot: %d\n\n", slotNumber+1);

            // Hold thread ID
            pthread_t threads[32];
            // Rotate n and create a thread for that rotated number
            for (int i = 0; i < 32; i++){
                // Rotate n by i bits
                // Create data struct to send to thread
                struct ThreadData data = {rightRotate(n, i), slotNumber};
                // Create thread
                if (pthread_create(&threads[i], NULL, &factorise, (void*) &data) != 0){
                    printError("Failed to create server thread.");
                }
                // Sleep buffer between each thread - can be used if threads are too fast
                usleep(10);
            }
        }
        // Tell client that server has read data & threads have been created
        ShmPtr->clientFlag = 0;
    }
    
    // Wait for active threads to finish
    while (ShmPtr->activeQueries > 0);
    // Process disconnection
    printLog("Disconnecting...");

    // Close, unlink + free semaphores
    for (int i = 0; i < 10; i++){
        sem_close(full[i]);
        sem_close(empty[i]);
        sem_unlink(sem1[i]);
        sem_unlink(sem2[i]);
    }

    // Detach and remove/destroy shared memory
    printLog("Detaching memory...");
    shmdt((void*) ShmPtr);
    printLog("Destroying shared memory...");
    shmctl(ShmId, IPC_RMID, NULL);
    // Close server
    printLog("Server closing.");
    return 0;
}

// Rotate unsigned integer right
unsigned int rightRotate(unsigned int n, unsigned int bits){
    // Shift n by specified bits right
    return (n >> bits)|(n << 32 - bits);
}

// Producer function
void* factorise(void* args){
    // Retrieve data sent from main thread
    struct ThreadData *data = (struct ThreadData*) args;
    int slotNumber = data->slotNumber;
    unsigned int n = data->n;
    // Set first initial factor
    int factor = 2;

    // Calculate factors using trial division algorithm
    // While factors are remaining
    while (n > 1){
        // Check if factor exists (no remainder)
        if (n % factor == 0){
            // Factor found - wait for slot to be free then write
            sem_wait(empty[slotNumber]);
            // Assign data to slot
            ShmPtr->slot[slotNumber] = factor;
            // New data is availible for client to read
            ShmPtr->serverFlag[slotNumber] = 1;
            // Post to client
            sem_post(full[slotNumber]);
            // Wait for client to respond
            while (ShmPtr->serverFlag[slotNumber] == 1);
            // Divide factor out of n
            n /= factor;
        } else {
            // Add one to factor, then retry
            factor++;
        }
    }

    // All factors found, write to slot that thread is finished
    // Wait for slot to be free
    sem_wait(empty[slotNumber]);
    // Thread finished
    ShmPtr->slot[slotNumber] = -1;
    // New data is availible for client read
    ShmPtr->serverFlag[slotNumber] = 1;
    // Post to client
    sem_post(full[slotNumber]);
    // Wait for client to respond
    while (ShmPtr->serverFlag[slotNumber] == 1);

    return NULL;
}

void* testmode(void* args){
    // Get thread number from data
    struct ThreadData *data = (struct ThreadData*) args;
    unsigned int n = data->n;
    // Send numbers back to client
    for (int i = 0; i < 10; i++){
        // Wait for write to be free
        sem_wait(empty[0]);
        // Write to number
        ShmPtr->number = (n*10)+i;
        // Set client flag
        ShmPtr->clientFlag = 0;
        // Post to client
        sem_post(full[0]);
        // Wait for client to read
        while (ShmPtr->clientFlag == 0);
    }

    // Test mode complete, let client know
    // Wait for write to be free
    sem_wait(empty[0]);
    // Write to number -> 1000 indicates thread is finished
    ShmPtr->number = 1000;
    // Set client flag
    ShmPtr->clientFlag = 0;
    // Post to client
    sem_post(full[0]);
    // Wait for client to read
    while (ShmPtr->clientFlag == 0);

    return NULL;
}