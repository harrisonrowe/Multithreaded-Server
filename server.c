// For 
// cd Assignment2/Multithreaded-Server
// ./server 80
// ./client 80 HARRY-PC2

// For shared memory + semaphores
// cygserver-config
// cygrunsrv -S cygserver
#include "includes.h"

int main(int argc, char* argv[]){
    // Welcome text
    printf("\n");
    printLog("2803ICT Assignment 2: By Harry Rowe.");
    
    // Validate command line input
    if (argc != 2){
        printError("Insuffficient command line arguments.");
    }

    // Check port address
    if (!isdigit(*argv[1]) || atoi(argv[1]) < 1){
        printError("Please enter a valid port number.");
    }

    // Client and server messages
    char clientMsg[SIZE] = "";
    char serverMsg[SIZE] = "";
    // Client and server sockets
    struct sockaddr_in client, server;
    // Hold client and server socket descriptor entry
    int clientSocket = 0, serverSocket = 0;
    // Port/server variables
    int port = atoi(argv[1]);

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
    // Set flags in shared memory
    ShmPtr->clientStatus = 0;
    ShmPtr->clientFlag = 0;
    for (int i = 0; i < 10; i++){
        ShmPtr->serverFlag[i] = 0;
    }

    // Initiate server connection
    initServer(&server, &serverSocket, port);
    // Accept connection - Accept 1 client
    connectClient(&client, &clientSocket, serverSocket);

    // Server initiated
    while(1){
        
        // Await incomming data from client
        while (ShmPtr->clientFlag == 0);

        // Check if client wishes to quit
        if (ShmPtr->clientStatus == 1){
            // Process disconnection
            printLog("Client disconnected.");
            break;
        }

        // Check data sent from client
        printLog("Data recieved from client...");
        printf("\tClient = %u\n\n", ShmPtr->number);
        
        // Run trial division
        printLog("Running trial division...");
        trialDivision(ShmPtr->number);

        // Reset clientFlag
        ShmPtr->clientFlag = 0;
    }
    
    // Detach shared memory
    shmdt((void*) ShmPtr);
    // Close client socket
    close(clientSocket);
    // Close server socket
    printLog("Server closing.");
    close(serverSocket);
    return 0;
}