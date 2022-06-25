#ifndef EXTRACT_H
#define EXTRACT_H

#define MAX_EXTENSION 8

#include "../include/helper.h"

typedef enum EXTRACT_RET
{
  SUCCESS,
  FAILURE
} EXTRACT_RET;

EXTRACT_RET extract(bmpFile *bmp, InputParams inputParams, file *extractedFile);
uint32_t getSizeFromPointer(const uint8_t *pointer);

#endif