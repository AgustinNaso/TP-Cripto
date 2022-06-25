#include <openssl/evp.h>
#include "include/types.h"
#include "include/utils.h"
#include <stdio.h>
#include <string.h>

const EVP_CIPHER *getCipherTypeForAES128(ENCRYPTION_MODE mode);
size_t getKeyLength(ENCRYPTION encryption);
const EVP_CIPHER *getCipherTypeForAES192(ENCRYPTION_MODE mode);
const EVP_CIPHER *getCipherTypeForAES256(ENCRYPTION_MODE mode);
const EVP_CIPHER *getCipherTypeForDES(ENCRYPTION_MODE mode);

uint8_t *decryptMessage(const uint8_t *ciphertext, InputParams inputParams, size_t size)
{
    EVP_CIPHER_CTX *context;
    const EVP_CIPHER *cipher;

    switch (inputParams.encryption)
    {
    case AES_128:
        cipher = getCipherTypeForAES128(inputParams.mode);
        break;
    case AES_192:
        cipher = getCipherTypeForAES192(inputParams.mode);
        break;
    case AES_256:
        cipher = getCipherTypeForAES256(inputParams.mode);
        break;
    case DES:
        cipher = getCipherTypeForDES(inputParams.mode);
        break;
    default:
        break;
    }

    uint8_t *decryption = calloc(size, 1);
    size_t keyLength = getKeyLength(inputParams.encryption);
    uint8_t *key = malloc(keyLength);
    uint8_t *iv = malloc(keyLength);

    EVP_BytesToKey(cipher,EVP_sha256(), NULL, inputParams.password, (int)strlen((char *)inputParams.password), 1, key, iv);

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
    // printf("\n%s\n",(char *) ciphertext);
    if (EVP_DecryptUpdate(context, decryption, &length, ciphertext, size) != 1)
        errorHandler("decrypt ciphertext");
     printf("\n DECRIPTION\n %s\n",(char *) decryption);
    /*
     * Finalize the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if (EVP_DecryptFinal_ex(context, decryption + length, &length) <= 0)
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
        return EVP_aes_128_cbc();
    default:
        return NULL;
    }
}

const EVP_CIPHER *getCipherTypeForAES192(ENCRYPTION_MODE mode)
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
    default:
        return NULL;
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
    default:
        return NULL;
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
    default:
        return NULL;
    }
}
