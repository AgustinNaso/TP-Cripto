#ifndef UTILS_H
#define UTILS_H

// #include <openssl/evp.h>
// #include "types.h"
#define GET_NTH_LSB(byte, n) ((byte >> (n-1) & 1))
#define GET_INT_FROM_2_BITS(bit1, bit2) (2*bit1 + bit2)
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

const char* getFileExtension(const char * path);
int modifyBit(unsigned char byte, int position, int bit);



#endif