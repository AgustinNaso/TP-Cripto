#include "../include/extract.h"
#include <stdlib.h>
#include <stdio.h>

void extractMessageFromLSB1(const uint8_t *data, uint8_t *extractedFile, size_t size) ;
void extractExtensionFromLSB1(const uint8_t *data, uint8_t *extension, size_t size);
void extractMessageFromLSB4(const uint8_t *data, file *extractedFile);
void extractExtensionFromLSB4(const uint8_t *data, file *extractedFile);

EXTRACT_RET extract(bmpFile *bmp, InputParams inputParams, file *extractedFile) 
{
  uint32_t currentEmbeddedSize = extractEmbeddedSize(bmp, inputParams);

  if (currentEmbeddedSize == SIZE_ERROR) 
  {
    printf("Error: unable to extract file with specified steganography algorithm\n");
    return FAILURE;
  }
  printf("img size %d\n", currentEmbeddedSize);

  extractedFile->data = malloc(currentEmbeddedSize);
  extractedFile->size = currentEmbeddedSize;
  extractedFile->extension = calloc(MAX_EXTENSION, 1);

  switch (inputParams.stego) 
  {
    case LSB1:
      extractMessageFromLSB1(bmp->data + BYTES_FOR_SIZE, extractedFile->data, extractedFile->size);
      extractExtensionFromLSB1(bmp->data + BYTES_FOR_SIZE, extractedFile->extension, extractedFile->size);
      break;
    case LSB4:
      extractMessageFromLSB4(bmp->data + 8, extractedFile);
      extractExtensionFromLSB4(bmp->data + 8, extractedFile);
      break;
    case LSBI:
      break;
  }
  
  return SUCCESS;
}

void extractMessageFromLSB1(const uint8_t *data, uint8_t *extractedFile, size_t size) 
{
  for (size_t i = 0; i < size; i++) 
  {
    uint8_t extractedByte = extractByteFromLSB1(i, data);
    extractedFile[i] = extractedByte;
    printf("extr: %zu %d\n", i, extractedFile[i]);
  }
}

void extractExtensionFromLSB1(const uint8_t *data, uint8_t *extension, size_t size) 
{
  size_t i = size, j = 0;
  uint8_t extractedByte = 1;

  while (i < size + 10) 
  {
    extractedByte = extractByteFromLSB1(i++, data);
    extension[j++] = extractedByte;
    printf("%zu %d\n", i-1, extension[j - 1]);
  }
}

void extractMessageFromLSB4(const uint8_t *data, file *extractedFile) 
{
  for (size_t i = 0, j = 0; j < extractedFile->size; i += 2, j++) 
  {
    uint8_t extractedByte = extractByteFromLSB4(i, data);
    extractedFile->data[j] = extractedByte;
  }
}

void extractExtensionFromLSB4(const uint8_t *data, file *extractedFile) 
{
  size_t i = extractedFile->size * 2, j = 0;
  uint8_t extractedByte = 1;

  while (extractedByte != 0) 
  {
    extractedByte = extractByteFromLSB1(i, data);
    extractedFile->extension[j++] = extractedByte;
    i += 2;
  }
}