#include "includes.h"

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

