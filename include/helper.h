#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>
#include <stddef.h>
#include "../include/fileHandler.h"
#include "../include/parser.h"

#define BYTES_FOR_SIZE 4
#define BIT_SIZE 8
#define SIZE_ERROR -1

size_t extractEmbeddedSize(bmpFile *bmp, InputParams *inputParams);
uint8_t extractByteFromLSB1(size_t byte, const uint8_t *data);

#endif