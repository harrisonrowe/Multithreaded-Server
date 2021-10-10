#include "includes.h"

// Server functions

// Rotate unsigned integer right
unsigned int rightRotate(unsigned int n, unsigned int bits){
    // Shift n by specified bits right
    return (n >> bits)|(n << 32 - bits);
}


