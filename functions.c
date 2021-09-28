#include "includes.h"

// Program functions

// Tokenise string
void tokenString(char*** msg, char originalMsg[SIZE], int* size){
    // Get command as array of strings, using token from strtok
    char* token = strtok(originalMsg, " ");
    // Store each command as a ragged array using array of char pointers
    while (token != NULL){
        // Reallocate space for each string, then allocate number of characters within the string
        (*msg) = (char**) realloc((*msg), ((*size)+1)*sizeof(**msg));
        (*msg)[(*size)] = malloc(sizeof(char)*strlen(token));
        // Assign token string, move to next using space as the delimiter
        (*msg)[(*size)++] = token;
        token = strtok(NULL, " "); // Tokenise by spaces
    }
}

// Print message to console
void printLog(char* msg){
    printf("\t\033[0;35mLOG:\033[0m %s\n\n", msg);
}

// Print error message to console
void printError(char* msg){
    printf("\t\033[0;31mERROR:\033[0m %s\n\n", msg);
    exit(0);
}
