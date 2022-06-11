#include "../include/helper.h"
#include "../include/types.h"

size_t extractSizeFromLSB1(bmpFile *bmp);
size_t extractSizeFromLSB4(bmpFile *bmp);
size_t extractSizeFromLSBI(bmpFile *bmp);

size_t extractEmbeddedSize(bmpFile *bmp, InputParams *inputParams) 
{
  if (inputParams->stego == LSB1) 
  {
    return extractSizeFromLSB1(bmp);
  } else if (inputParams->stego == LSB4) 
  {
    return extractSizeFromLSB4(bmp);
  }
  return extractSizeFromLSBI(bmp);
}

size_t extractSizeFromLSB1(bmpFile *bmp) 
{
  uint32_t size = 0;

  for (int i = 0; i < BYTES_FOR_SIZE; i++) 
  {
    uint32_t extractedByte = extractByteFromLSB1(i, bmp->data);
    size |= extractedByte << (8u * (3u - i));
  }

  if (size * 8 != bmp->imageHeader->imageSize) return SIZE_ERROR;

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

  if (size * 2 != bmp->imageHeader->imageSize) return SIZE_ERROR;

  return size;
}

size_t extractSizeFromLSBI(bmpFile *bmp) 
{
  uint32_t size = 0;
  return size;
}

uint8_t extractByteFromLSB1(size_t byte, const uint8_t *data) 
{
  uint8_t extractedByte = 0;
  for (int i = 0; i < BIT_SIZE; i++) 
  {
    uint8_t bit = data[byte * 8 + i] & 1u;
    extractedByte |= (uint8_t) (bit << (7u + i));
  }
  return extractedByte;
}

uint8_t extractByteFromLSB4(size_t byte, const uint8_t *data)
{
  uint8_t extractedByte = 0;

  // add first four bits shifted left four places
  extractedByte |= (uint8_t) ((data[byte] & 0x0Fu) << 4u);
  // add second four bits
  extractedByte |= (data[byte + 1] & 0x0Fu);

  return extractedByte;
}