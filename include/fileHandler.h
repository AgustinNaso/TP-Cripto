#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <stdint.h>
#include <stddef.h>
#define HEADER_SIZE 54
#define BITS_PER_PIXEL 24

typedef struct bmpFileHeader {
    uint16_t    type;
    uint32_t    size;
    uint16_t    reserved1;
    uint16_t    reserved2;
    uint32_t    offset;
} bmpFileHeader;

typedef struct bmpImageHeader {
    uint32_t    headerSize;        // 4 bytes
    uint32_t    width;              // 4 bytes
    uint32_t    height;             // 4 bytes
    uint16_t    planes;             // 2 bytes
    uint16_t    bitCount;          // 2 bytes
    uint32_t    compression;        // 4 bytes
    uint32_t    imageSize;         // 4 bytes
    uint32_t    ppmX;              // 4 bytes
    uint32_t    ppmY;              // 4 bytes
    uint32_t    colorsUsed;           // 4 bytes
    uint32_t    importantColors;      // 4 bytes
} bmpImageHeader;

typedef struct rgbData {
    unsigned char r, g, b;
} rgbData;

typedef struct bmpFile
{
  bmpFileHeader *header;
  bmpImageHeader *imageHeader;
  uint8_t *data;
} bmpFile;

typedef struct file
{
    uint8_t *data;
    size_t size;
    uint8_t *extension;
} file;

bmpFile *parseBmpFile(char *bmpPath);
void writeMessageToOutput(file *extractedFile, const char *outputFileName);

#endif