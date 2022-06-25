#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>
#include <stddef.h>
#include "../include/fileHandler.h"
#include "../include/parser.h"
#include "../include/utils.h"

#define BYTES_FOR_SIZE 4
#define BIT_SIZE 8
#define SIZE_ERROR -1
#define GROUPS 4

void extractActiveGroupBits(const uint8_t *data, int groups[4]);
size_t extractSizeFromLSB1(bmpFile *bmp);
size_t extractSizeFromLSB4(bmpFile *bmp);
size_t extractSizeFromLSBI(bmpFile *bmp, int groups[4]);
uint8_t extractByteFromLSB1(size_t byte, const uint8_t *data);
uint8_t extractByteFromLSB4(size_t byte, const uint8_t *data);
uint8_t extractByteFromLSBI(size_t byte, const uint8_t *data, int groups[4]);
void extractFileExtension(file *extractedFile, const uint8_t *data);

#endif