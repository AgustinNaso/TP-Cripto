#include "../include/extract.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void extractMessageFromLSB1(const uint8_t *data, uint8_t *extractedFile, size_t size) ;
void extractExtensionFromLSB1(const uint8_t *data, uint8_t *extension, size_t size);
void extractMessageFromLSB4(const uint8_t *data, uint8_t *extractedFile, size_t size);
void extractExtensionFromLSB4(const uint8_t *data, uint8_t *extension, size_t size);
void extractMessageFromLSBI(const uint8_t *data, uint8_t *extractedFile, size_t size, int groups[4]);
void extractExtensionFromLSBI(const uint8_t *data, uint8_t *extension, size_t size, int groups[4]);
uint8_t *decryptMessage(const uint8_t *ciphertext, InputParams inputParams, size_t size);

EXTRACT_RET extract(bmpFile *bmp, InputParams inputParams, file *extractedFile) 
{

  uint32_t currentEmbeddedSize = 0;
  int groups[GROUPS] = {-1};

  switch (inputParams.stego) 
  {
    case LSB1:
      currentEmbeddedSize = extractSizeFromLSB1(bmp);
    case LSB4:
      currentEmbeddedSize = extractSizeFromLSB4(bmp);
      break;
    case LSBI:
      extractActiveGroupBits(bmp->data, groups);
      currentEmbeddedSize = extractSizeFromLSBI(bmp, groups);
      break;
  }

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

uint8_t *decryptMessage(const uint8_t *ciphertext, InputParams inputParams, size_t size)
{
    EVP_CIPHER_CTX *context;
    const EVP_CIPHER *cipher;

    switch (inputParams.encryption)
    {
        case AES_128:
            cipher = getCipherTypeForAES128(inputParams.mode);
        case AES_192:
            cipher = getCipherTypeForAES192(inputParams.mode);
        case AES_256:
            cipher = getCipherTypeForAES256(inputParams.mode);
        case DES:
            cipher = getCipherTypeForDES(inputParams.mode);
    }

    uint8_t *decryption = calloc(size, 1);
    size_t keyLength = getKeyLength(inputParams.encryption);
    uint8_t *key = malloc(keyLength);
    uint8_t *iv = malloc(keyLength);

    EVP_BytesToKey(cipher, EVP_md5(), NULL, inputParams.password, (int)strlen((char *)inputParams.password), 1, key, iv);

    /* Create and initialize the context */
    if (!(context = EVP_CIPHER_CTX_new()))
      errorHandler("create cipher context");

    /*
     * Initialize the decryption operation. 
     * IMPORTANT - ensure you use a key and IV size appropriate for your cipher
     */
    if (EVP_DecryptInit_ex(context, cipher, NULL, key, iv) != 1)
      errorHandler("initialize decryption operation");

    int length;
    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     */
    if (EVP_DecryptUpdate(context, decryption, &length, ciphertext, size) != 1)
      errorHandler("decrypt ciphertext");

    /*
     * Finalize the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if (EVP_DecryptFinal_ex(context, decryption + length, &length) != 1)
      errorHandler("finalize decryption");

    /* Clean up */
    EVP_CIPHER_CTX_free(context);

    free(key);
    free(iv);

    return decryption;
}

size_t getKeyLength(ENCRYPTION encryption)
{
    switch (encryption)
    {
        case AES_128:
            return 16;
        case AES_192:
            return 24;
        case AES_256:
            return 32;
        case DES:
            return 8;
        case NONE:
        default:
            return 0;
    }
}

const EVP_CIPHER *getCipherTypeForAES128(ENCRYPTION_MODE mode)
{
    switch (mode)
    {
        case ECB:
            return EVP_aes_128_ecb();
        case CFB:
            return EVP_aes_128_cfb1();
        case OFB:
            return EVP_aes_128_ofb();
        case CBC:
            return EVP_aes_128_cbc();   // TODO: Comparar con otros CBC
        default:
            break;
    }

    return NULL;
}

const EVP_CIPHER *getCipherTypeForAES192(ENC_MODE mode)
{
    switch (mode)
    {
        case ECB:
            return EVP_aes_192_ecb();
        case CFB:
            return EVP_aes_192_cfb1();
        case OFB:
            return EVP_aes_192_ofb();
        case CBC:
            return EVP_aes_192_cbc();
    }
}

const EVP_CIPHER *getCipherTypeForAES256(ENCRYPTION_MODE mode)
{
    switch (mode)
    {
        case ECB:
            return EVP_aes_256_ecb();
        case CFB:
            return EVP_aes_256_cfb1();
        case OFB:
            return EVP_aes_256_ofb();
        case CBC:
            return EVP_aes_256_cbc();
    }
}

const EVP_CIPHER *getCipherTypeForDES(ENCRYPTION_MODE mode)
{
    switch (mode)
    {
        case ECB:
            return EVP_des_ecb();
        case CFB:
            return EVP_des_cfb1();
        case OFB:
            return EVP_des_ofb();
        case CBC:
            return EVP_des_cbc();
    }
}
