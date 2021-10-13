#include "includes.h"

// Semaphore classes used in threads
sem_t *full[10];
sem_t *empty[10];

// Mutex for client side message buffer
pthread_mutex_t mutexBuffer;

// Pointer to shared memory
struct Memory *ShmPtr; 

int main(int argc, char* argv[]){
    // Welcome text 
    printf("\n");
    printLog("2803ICT Assignment 2: By Harry Rowe.");

    // Seed random number
    srand(time(0));

    // SHARED MEMORY SETUP //

    // Set up shared memory
    key_t ShmKey = ftok(".", 'a'); // Shared memory key using token of current path
    int ShmId = shmget(ShmKey, sizeof(struct Memory), 0666); // Create shared memory
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
        printLog("Client has attached shared memory.");
    }

    // SEMAPHORE & MUTEX SETUP //

    // Init mutex
    pthread_mutex_init(&mutexBuffer, NULL);
    // Init semaphores
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
        // Append i to each prefix for unique name
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
        // Handle if empty val or mutex val starts at 0
        int emptyVal;
        sem_getvalue(empty[i], &emptyVal);
        while (emptyVal < 1){
            sem_post(empty[i]);
            emptyVal++;
        }
    }

    // TIMEOUT FILEDESCRIPTORS & STRUCTS //

    fd_set fds;
    struct timeval tv;
    int retval;

    // Client ready
    printLog("Client setup.");
    // Request users to enter 32-bit integers
    printLog("Please continually enter unsigned 32-bit integers or 'q' to quit:");

    // Await and send client input to server
    while(1){
        
        // Request user for 32 bit integer
        char clientMessage[SIZE] = "";
        // Whilst no active queries and the client has no response in 500ms - print progress
        while (1){
            // Watch standard input for client query
            FD_ZERO(&fds);
            FD_SET(0, &fds);
            // Set timeout for 500ms input time
            tv.tv_sec = 0;
            tv.tv_usec = 500000;
            retval = select(1, &fds, NULL, NULL, &tv);

            if (retval == 0){
                // No response in 500ms, print progress report if no active queries
                if (ShmPtr->activeQueries == 0){
                    printProgress();
                }
            } else {
                // Client has input a query
                scanf("%[^\n]%*c", clientMessage);
                break;
            }
        }

        // Respond to client request
        // Check if client wishes to quit
        if (strcmp(clientMessage, "") == 0){
            printLog("Please enter a valid 32-bit integer.");
            // Clear scanf buffer
            while (getchar() != '\n');
        } else if (strcmp(clientMessage, "q") == 0){
            // Check if client wishes to quit
            printLog("Client requested to leave...");
            // Check active queries
            if (ShmPtr->activeQueries > 0){
                printLog("Quitting after threads finish...");
            }
            ShmPtr->clientFlag = -1;
            break;
        } else if (ShmPtr->activeQueries == 10){
            // Client has requested another query but server is busy.
            printLog("Server busy. Please await for a query to complete.");
        } else if (strcmp(clientMessage, "0") == 0){
            // Client has requested test mode
            if (ShmPtr->activeQueries != 0){
                // Queries still in progress
                printLog("Queries in progress - test mode not activated.");
            } else {
                // Run test mode
                printLog("Test mode activated.");
                // Update shared memory number
                ShmPtr->number = 0;
                // Indicate to server that new data is ready
                ShmPtr->clientFlag = 1;
                // Await response back from server
                while (ShmPtr->clientFlag == 1);
                // Create consumer thread
                pthread_t thread;
                void *ret;
                // Create thread
                if (pthread_create(&thread, NULL, &testConsumer, NULL) != 0){
                    printError("Failed to create client thread.");
                }
                // Join thread
                if (pthread_join(thread, &ret) != 0){
                    printError("Failed to join client thread");
                }
                printf("\n");
                printLog("Test mode complete.");
            }
        } else {
            // Pass integer to server through shared memory
            // Validate client input
            unsigned int num = validateClientInput(clientMessage);
            if (num == 0){
                printLog("Please enter valid input.");
                continue;
            }
            // Update shared memory number
            ShmPtr->number = num;
            // Indicate to server that new data is ready
            ShmPtr->clientFlag = 1;
            // Await response back from server
            while (ShmPtr->clientFlag == 1);
            // Set data to be sent to thread
            struct ThreadData data = {0, ShmPtr->number};
            // Allocate consumer for particular slot
            pthread_t thread;
            // Create thread
            if (pthread_create(&thread, NULL, &slotConsumer, (void*) &data) != 0){
                printError("Failed to create client thread.");
            }
        }
    }

    // Wait for active threads to finish
    while (ShmPtr->activeQueries > 0);

    // Close mutex
    pthread_mutex_destroy(&mutexBuffer);
    // Close and unlink semaphores
    for (int i = 0; i < 10; i++){
        sem_close(full[i]);
        sem_close(empty[i]);
        sem_unlink(sem1[i]);
        sem_unlink(sem2[i]);
    }
    // Detach shared memory
    shmdt((void*) ShmPtr);
    // Close client
    printLog("Client exiting.");
    return 0;
}

// Print query progress
void printProgress(){
    printf("\n> Progress: ");
    // Loop through each query
    for (int i = 0; i < 10; i++){
        printf("Q%d: %.2f%% ", i+1, ShmPtr->progress[i]);
    }
    printf("\n\n");
}

// Validate that the client has input a 32 bit signed integer
unsigned int validateClientInput(char clientMessage[SIZE]){
    // Check each character in string
    for (int i = 0; clientMessage[i] != '\0'; i++){
        if (i == 0 && clientMessage[i] == '-'){
            // Negative number entered
            return 0;
        } else if (!isdigit(clientMessage[i])){
            // Character is not a number
            return 0;
        }
    }
    // Ensure value assigned to n is between 1 and UINT max
    unsigned int n = atoi(clientMessage);
    if (n > UINT_MAX || n < 1){
        return 0;
    }
    // String validated, accept client input
    return n;
}

// Consumer function
void* slotConsumer(void* args){
    // Retrieve data sent from main thread
    struct ThreadData *data = (struct ThreadData*) args;
    // Save data from thread to prevent overwriting data
    int finishedThreads = 0;
    int slotNumber = data->slotNumber;
    // Start timer
    struct timeval begin, end;
    gettimeofday(&begin, 0);
    // Listen for server response loop
    while (1){
        // Await for slot buffer to be filled
        sem_wait(full[slotNumber]);
        // Assign factor
        int factor = ShmPtr->slot[slotNumber];
        // Indicate to server that data has been read
        ShmPtr->serverFlag[slotNumber] = 0;

        // Check if thread is finished
        if (factor == -1){
            // Update progress report by 32/100
            ShmPtr->progress[slotNumber] += 3.125;
            finishedThreads++;
            // If all 32 threads finished, destroy thread
            if (finishedThreads == 32){
                // All threads completed
                break;
            }
        } else if (factor > 0){
            // Print factor - use mutex lock to print to screen
            pthread_mutex_lock(&mutexBuffer);
            printf("\tData from query %d: %d\n", slotNumber+1, factor);
            pthread_mutex_unlock(&mutexBuffer);
            // To see multithreaded implementation, uncomment sleep(1) for sleep between each factor
            // sleep(1);
        }
        sem_post(empty[slotNumber]);
    }
    // All server threads for query finished
    // Add slot number onto stack
    ShmPtr->slotAllocation[++ShmPtr->slotAllocationCounter] = slotNumber;
    // Calculate time taken
    gettimeofday(&end, 0);
    long sec = end.tv_sec - begin.tv_sec;
    long ms = end.tv_usec - begin.tv_usec;
    double timer = sec + ms*1e-6;
    // Let client know that query has been completed
    pthread_mutex_lock(&mutexBuffer);
    printf("\n\tQuery %d finished\n\tTime Taken: %.3f s\n\n", slotNumber+1, timer);
    pthread_mutex_unlock(&mutexBuffer);
    // Indicate to server that data has been read
    ShmPtr->serverFlag[slotNumber] = 0;
    sem_post(empty[slotNumber]);
    // Decrease active queries 
    ShmPtr->activeQueries--;
    // Print progress report
    printProgress();

    return NULL;
}

void* testConsumer(void* args){
    // Store threads completed
    int finishedThreads = 0;
    // Loop through each reply from server
    while (1){
        // Wait for server input
        sem_wait(full[0]);
        // Assign number
        int testNumber = ShmPtr->number;
        // Indicate to server that data has been read
        ShmPtr->clientFlag = 1;

        // Check if thread has finshed
        if (testNumber == 1000){
            finishedThreads++;
            // All test threads finished
            if (finishedThreads == 30){
                sem_post(empty[0]);
                break;
            }
        } else {
            // Print test number
            pthread_mutex_lock(&mutexBuffer);
            printf("\tData from test thread: %d\n", testNumber);
            pthread_mutex_unlock(&mutexBuffer);
            // Wait time - random choice between 10ms and 100ms
            int random = rand() % 2;
            // Wait specified time
            if (random == 0){
                usleep(10);
            } else {
                usleep(100);
            }
        }
        // Post back to threads
        sem_post(empty[0]);
    }

    return NULL;
}