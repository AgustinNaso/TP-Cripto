#ifndef UTILS_H
#define UTILS_H

// #include <openssl/evp.h>
// #include "types.h"
#define GET_NTH_LSB(byte, n) ((byte >> (n-1) & 1))
#define GET_INT_FROM_2_BITS(bit1, bit2) (2*bit1 + bit2)

const char* getFileExtension(const char * path);
int modifyBit(unsigned char byte, int position, int bit);



#endif