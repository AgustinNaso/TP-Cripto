#ifndef DECRYPT_H
#define DECRYPT_H

#include "types.h"

uint8_t *decryptMessage(const uint8_t *ciphertext, InputParams inputParams, size_t size);

#endif