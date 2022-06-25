#include "../include/extract.h"
#include "../include/decrypt.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void extractMessageFromLSB1(const uint8_t *data, uint8_t *extractedFile, size_t size) ;
void extractExtensionFromLSB1(const uint8_t *data, uint8_t *extension, size_t size);
void extractMessageFromLSB4(const uint8_t *data, uint8_t *extractedFile, size_t size);
void extractExtensionFromLSB4(const uint8_t *data, uint8_t *extension, size_t size);
void extractMessageFromLSBI(const uint8_t *data, uint8_t *extractedFile, size_t size, int groups[4]);
void extractExtensionFromLSBI(const uint8_t *data, uint8_t *extension, size_t size, int groups[4]);

EXTRACT_RET extract(bmpFile *bmp, InputParams inputParams, file *extractedFile) 
{

  uint32_t currentEmbeddedSize = 0;
  int groups[GROUPS] = {-1};
  printf(" param %d", inputParams.stego);

  switch (inputParams.stego) 
  {
    case LSB1:
      currentEmbeddedSize = extractSizeFromLSB1(bmp);
      break;
    case LSB4:
      currentEmbeddedSize = extractSizeFromLSB4(bmp);
      break;
    case LSBI:
      extractActiveGroupBits(bmp->data, groups);
      currentEmbeddedSize = extractSizeFromLSBI(bmp, groups);
      break;
  }
  printf("%d %d", currentEmbeddedSize, bmp->imageHeader->imageSize );
  if (currentEmbeddedSize == SIZE_ERROR) 
  {
    printf("Error: unable to extract file with specified steganography algorithm\n");
    return FAILURE;
  }

  if (inputParams.encryption != NONE) {

    uint8_t *ciphertext = malloc(currentEmbeddedSize);

    switch (inputParams.stego)
    {
    case LSB1:
      extractMessageFromLSB1(bmp->data + BYTES_FOR_SIZE * 8, ciphertext, currentEmbeddedSize);
      break;
    case LSB4:
      extractMessageFromLSB4(bmp->data + BYTES_FOR_SIZE * 2, ciphertext, currentEmbeddedSize);
      break;
    case LSBI:
      extractMessageFromLSBI(bmp->data + GROUPS + BYTES_FOR_SIZE * 8, ciphertext, currentEmbeddedSize, groups);
      break;
    }

    uint8_t *plaintext = decryptMessage(ciphertext, inputParams, currentEmbeddedSize);
    
    extractedFile->size = getSizeFromPointer(plaintext);
    extractedFile->data = calloc(extractedFile->size, 1);
    extractedFile->extension = calloc(MAX_EXTENSION, 1);
    memcpy(extractedFile->data, plaintext + BYTES_FOR_SIZE, extractedFile->size);
    extractFileExtension(extractedFile, plaintext + BYTES_FOR_SIZE + extractedFile->size);

  } else {

    extractedFile->data = malloc(currentEmbeddedSize);
    extractedFile->size = currentEmbeddedSize;
    extractedFile->extension = calloc(MAX_EXTENSION, 1);

    switch (inputParams.stego) 
    {
      case LSB1:
        // Add 4 * 8 because for each byte of size we need 8 bytes of bmp data
        extractMessageFromLSB1(bmp->data + BYTES_FOR_SIZE * 8, extractedFile->data, extractedFile->size);
        extractExtensionFromLSB1(bmp->data + BYTES_FOR_SIZE * 8, extractedFile->extension, extractedFile->size);
        break;
      case LSB4:
      // Add 4 * 2 because for each byte of size we need 2 bytes of bmp data
        extractMessageFromLSB4(bmp->data + BYTES_FOR_SIZE * 2, extractedFile->data, extractedFile->size);
        extractExtensionFromLSB4(bmp->data + BYTES_FOR_SIZE * 2, extractedFile->extension, extractedFile->size);
        break;
      case LSBI:
        extractMessageFromLSBI(bmp->data + GROUPS + BYTES_FOR_SIZE * 8, extractedFile->data, extractedFile->size, groups);
        extractExtensionFromLSBI(bmp->data + GROUPS + BYTES_FOR_SIZE * 8, extractedFile->extension, extractedFile->size, groups);
        break;
    }
  }

  return SUCCESS;
}

void extractMessageFromLSB1(const uint8_t *data, uint8_t *extractedFile, size_t size) 
{
  for (size_t i = 0; i < size; i++) 
  {
    uint8_t extractedByte = extractByteFromLSB1(i, data);
    extractedFile[i] = extractedByte;
  }
}

void extractExtensionFromLSB1(const uint8_t *data, uint8_t *extension, size_t size) 
{
  size_t i = size, j = 0;
  uint8_t extractedByte = 1;

  while (extractedByte != 0) 
  {
    extractedByte = extractByteFromLSB1(i++, data);
    extension[j++] = extractedByte;
  }
}

void extractMessageFromLSB4(const uint8_t *data, uint8_t *extractedFile, size_t size)
{
  for (size_t i = 0, j = 0; j < size; i += 2, j++) 
  {
    uint8_t extractedByte = extractByteFromLSB4(i, data);
    extractedFile[j] = extractedByte;
  }
}

void extractExtensionFromLSB4(const uint8_t *data, uint8_t *extension, size_t size) 
{
  size_t i = size * 2, j = 0;
  uint8_t extractedByte = 1;

  while (extractedByte != 0) 
  {
    extractedByte = extractByteFromLSB4(i, data);
    extension[j++] = extractedByte;
    i += 2;
  }
}

void extractMessageFromLSBI(const uint8_t *data, uint8_t *extractedFile, size_t size, int groups[4]) 
{
  for (size_t i = 0; i < size; i++) 
  {
    uint8_t extractedByte = extractByteFromLSBI(i, data, groups);
    extractedFile[i] = extractedByte;
  }
}

void extractExtensionFromLSBI(const uint8_t *data, uint8_t *extension, size_t size, int groups[4]) 
{
  size_t i = size, j = 0;
  uint8_t extractedByte = 1;

  while (extractedByte != 0) 
  {
    extractedByte = extractByteFromLSBI(i++, data, groups);
    extension[j++] = extractedByte;
  }
}
