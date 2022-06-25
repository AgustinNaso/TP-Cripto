#ifndef ENCRYPT_H
#define ENCRYPT_H

char * encrypt(const unsigned char *password, const unsigned char *toEncrypt, int cipherAlgo, int mode, int dataLen);

#endif
