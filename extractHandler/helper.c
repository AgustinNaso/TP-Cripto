#include "../include/helper.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t extractBitFromLSBI(size_t byte, const uint8_t *data, int groups[4]);

size_t extractSizeFromLSB1(bmpFile *bmp) 
{
  uint32_t size = 0;

  for (int i = 0; i < BYTES_FOR_SIZE; i++) 
  {
    uint32_t extractedByte = extractByteFromLSB1(i, bmp->data);
    size |= extractedByte << (8u * (3u - i));
  }

  if (size * 8 > bmp->imageHeader->imageSize) return SIZE_ERROR;

  return size;
}

size_t extractSizeFromLSB4(bmpFile *bmp) 
{
  uint32_t size = 0;

  for (int i = 0, j = 0; j < BYTES_FOR_SIZE; i += 2, j++) 
  {
    uint32_t extractedByte = extractByteFromLSB4(i, bmp->data);
    size |= extractedByte << (8u * (3u - j));
  }

  if (size * 2 > bmp->imageHeader->imageSize) return SIZE_ERROR;

  return size;
}

size_t extractSizeFromLSBI(bmpFile *bmp, int groups[4]) 
{
  uint32_t size = 0;

  for (int i = 0; i < BYTES_FOR_SIZE; i++) 
  { 
    uint32_t extractedByte = extractByteFromLSBI(i, bmp->data + GROUPS, groups);
    size |= extractedByte << (8u * (3u - i));
  }

  printf("Size : %d %d\n", size, bmp->imageHeader->imageSize);

  if (size * 8 > bmp->imageHeader->imageSize) return SIZE_ERROR;

  return size;
}

uint8_t extractByteFromLSB1(size_t byte, const uint8_t *data) 
{
  uint8_t extractedByte = 0;

  for (int i = 0; i < BIT_SIZE; i++) 
  {
    uint8_t bit = data[byte * 8 + i] & 1u;
    extractedByte |= (uint8_t) (bit << (7u - i));
  }

  return extractedByte;
}

uint8_t extractByteFromLSB4(size_t byte, const uint8_t *data)
{
  uint8_t extractedByte = 0;

  // add first four bits shifted left four places
  extractedByte |= (uint8_t) ((data[byte] & 0x0Fu) << 4u);
  // add second four bits
  extractedByte |= (uint8_t) (data[byte + 1] & 0x0Fu);

  return extractedByte;
}

uint8_t extractByteFromLSBI(size_t byte, const uint8_t *data, int groups[4]) 
{
  uint8_t extractedByte = 0;

  for (int i = 0; i < BIT_SIZE; i++) 
  {
    uint8_t bit = extractBitFromLSBI(byte * 8 + i, data, groups);
    printf("real bit: %d\n", bit);
    extractedByte |= (uint8_t) (bit << (7u - i));
  }

  return extractedByte;
}

uint8_t extractBitFromLSBI(size_t byte, const uint8_t *data, int groups[4]) 
{
  printf("%d\n", data[byte]);
  uint8_t bit = GET_NTH_LSB(data[byte], 1);
  uint8_t secondLSB = GET_NTH_LSB(data[byte], 2);
  uint8_t thirdLSB = GET_NTH_LSB(data[byte], 3);

  printf("grupo %d %d %d\n", GET_NTH_LSB(data[byte], 1), GET_INT_FROM_2_BITS(thirdLSB, secondLSB), groups[GET_INT_FROM_2_BITS(thirdLSB, secondLSB)]);

  if (groups[GET_INT_FROM_2_BITS(thirdLSB, secondLSB)] == 1) return (~bit) & 1u;

  return bit;
}

void extractActiveGroupBits(const uint8_t *data, int groups[4]) 
{
  for (int i = 0; i < GROUPS; i++) {
    groups[i] = data[i] & 1u;
  }
}