#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>

#define SIZE_BYTES 4

typedef enum INPUT_RET 
{
  OK = 0,
  OPERATION_ERROR,
  MISSING_PARAMETER,
  MISSING_ARGUMENT,
  MISSING_OPERATION,
  NOT_BMP_FILE,
  PARAMETER_NOT_ALLOWED,
  STEGO_NOT_ALLOWED,
  ENCRYPTION_NOT_ALLOWED,
  MODE_NOT_ALLOWED
} INPUT_RET;

typedef enum OPTIONS
{
  EMB,
  EXTR,
  IN,
  CARRIER,
  OUT,
  STEGO,
  ENC,
  MODE,
  PASSWORD,
  END
} OPTIONS;

typedef enum STEGO_ALGORITHM 
{
  LSB1,
  LSB4,
  LSBI
} STEGO_ALGORITHM;

typedef enum OPERATION 
{
  EMBED,
  EXTRACT
} OPERATION;

typedef enum ENCRYPTION
{
    NONE,
    AES_128,
    AES_192,
    AES_256,
    DES
} ENCRYPTION;

typedef enum ENCRYPTION_MODE
{
    ECB,
    CFB,
    OFB,
    CBC
} ENCRYPTION_MODE;

typedef enum BMP_CHECK
{
  BMP_OK,
  BMP_ERROR
} BMP_CHECK;

typedef struct InputParams 
{
  OPERATION operation;
  char * inputFile; // -in
  char * outputFile; // -out
  char * carrierFile; // -p
  STEGO_ALGORITHM stego;
  ENCRYPTION encryption;
  ENCRYPTION_MODE mode;
  uint8_t * password;
  INPUT_RET status;
} InputParams;

#endif