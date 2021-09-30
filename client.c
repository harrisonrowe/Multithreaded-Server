#include "includes.h"

int main(int argc, char* argv[]){
    // Welcome text 
    printf("\n");
    printLog("2803ICT Assignment 2: By Harry Rowe.");
    
    // Validate command line input
    if (argc != 3){
        printError("Insuffficient command line arguments.");
    }

    // Client socket structure
    struct sockaddr_in server;
    // Used for bind of client socket
    int res = 0;
    // Hold client socket descriptor entry
    int clientSocket = 0;
    // Port/server variables
    int port = atoi(argv[1]);
    // Command line variables
    char serverName[SIZE];
    // Copy from command line
    strcpy(serverName, argv[2]);

    // Set up shared memory
    key_t ShmKey = ftok(".", 'a'); // Shared memory key using token of current path
    int ShmId = shmget(ShmKey, sizeof(struct Memory), 0666); // Create shared memory
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
        printLog("Client has attached shared memory.");
    }

    // Initiate client
    initClient(&server, &clientSocket, port, serverName);
    
    // If communication with server is successful, run a loop, listening for messages
    while(1){

        // Request user for 32 bit integer
        char clientMessage[SIZE] = "";
        printLog("Please enter an unsigned 32-bit integer:");
        printf("> ");
        scanf("%[^\n]%*c", clientMessage);
        printf("\n");

        // Check if client wishes to quit
        if (strcmp(clientMessage, "q") == 0){
            // Process disconnection - send to server
            printLog("Disconnecting...");
            ShmPtr->clientFlag = 1;
            ShmPtr->clientStatus = 1;
            break;
        }

        // Validate client input
        unsigned int num = validateClientInput(clientMessage);
        if (num == 0){
            printLog("Please enter valid input.");
            continue;
        }
        // Update shared memory number
        ShmPtr->number = num;
        // Notify server that client has updated shared memory
        ShmPtr->clientFlag = 1;
        
        // Wait whilst server reads data
        while (ShmPtr->clientFlag == 1);
        

    }

    // Detach and remove/destroy shared memory
    shmdt((void*) ShmPtr);
    shmctl(ShmId, IPC_RMID, NULL);
    // Close client socket
    close(clientSocket);
    return 0;
}