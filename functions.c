#include "includes.h"

// Program functions

// Print message to console
void printLog(char* msg){
    printf("\t\033[0;35mLOG:\033[0m %s\n\n", msg);
}

// Print error message to console
void printError(char* msg){
    printf("\t\033[0;31mERROR:\033[0m %s\n\n", msg);
    exit(0);
}
