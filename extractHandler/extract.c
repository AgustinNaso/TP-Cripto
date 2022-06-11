#include "../include/extract.h"
#include <stdlib.h>
#include <stdio.h>

void extractMessageFromLSB1(const uint8_t *data, file *extractedFile);
void extractExtensionFromLSB1(const uint8_t *data, file *extractedFile);
void extractMessageFromLSB4(const uint8_t *data, file *extractedFile);
void extractExtensionFromLSB4(const uint8_t *data, file *extractedFile);

EXTRACT_RET extract(bmpFile *bmp, InputParams *inputParams, file *extractedFile) 
{
  uint32_t currentEmbeddedSize = extractEmbeddedSize(bmp, inputParams);

  if (currentEmbeddedSize == SIZE_ERROR) 
  {
    printf("Error: unable to extract file with specified steganography algorithm\n");
    return FAILURE;
  }

  extractedFile->data = malloc(currentEmbeddedSize);
  extractedFile->size = currentEmbeddedSize;
  extractedFile->extension = calloc(MAX_EXTENSION, 1);

  switch (inputParams->stego) 
  {
    case LSB1:
      extractMessageFromLSB1(bmp->data + BYTES_FOR_SIZE * 8, extractedFile);
      extractExtensionFromLSB1(bmp->data + BYTES_FOR_SIZE * 8, extractedFile);
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

void extractMessageFromLSB1(const uint8_t *data, file *extractedFile) 
{
  for (size_t i = 0; i < extractedFile->size; i++) 
  {
    uint8_t extractedByte = extractByteFromLSB1(i, data);
    extractedFile->data[i] = extractedByte;
  }
}

void extractExtensionFromLSB1(const uint8_t *data, file *extractedFile) 
{
  size_t i = extractedFile->size, j = 0;
  uint8_t extractedByte = 1;

  while (extractedByte != 0) 
  {
    extractedByte = extractByteFromLSB1(i++, data);
    extractedFile->extension[j++] = extractedByte;
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