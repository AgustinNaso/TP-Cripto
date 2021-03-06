#include <stdio.h>
#include <stdlib.h>
#include <openssl/des.h>
#include <openssl/bio.h>
#include <string.h>
#include <openssl/evp.h>
#include "include/fileHandler.h"
#include "include/types.h"
#define BLOCK_SIZE 8
#define SUCCESS 0
#define FAILURE 1
#define MAX_ENCR_LENGTH 4096
#define KEY_LENGTH 16

int saveEncryptedData(unsigned char *out, int len, char *where)
{
    BIO *b64;
    BIO *bio;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_file());
    if (bio == NULL)
        return FAILURE;
    if (!BIO_write_filename(bio, where))
        return FAILURE;
    bio = BIO_push(b64, bio);
    BIO_write(bio, out, len);
    BIO_flush(bio);
    BIO_free_all(bio);
    return SUCCESS;
}

void showKey(unsigned char key[], int keySize)
{
    int i;
    for (i = 0; i < keySize; i++)
        printf("%0x", key[i]);
}

const EVP_CIPHER *getCipherAndMode(int algorithm, int mode)
{
    char cipherName[14] = {0};
    OpenSSL_add_all_ciphers();

    switch (algorithm)
    {
    case AES_128:
        strncpy(cipherName, "aes-128-", 9);
        break;
    case AES_192:
        strncpy(cipherName, "aes-192-", 9);
        break;
    case AES_256:
        strncpy(cipherName, "aes-256-", 9);
        break;
    case DES:
        strncpy(cipherName, "des-", 5);
        break;
    default:
        return NULL;
    }
    switch (mode)
    {
    case ECB:
        strncat(cipherName, "ecb", 4);
        break;
    case CFB:
        strncat(cipherName, "cfb1", 5);
        break;
    case OFB:
        strncat(cipherName, "ofb", 4);
        break;
    case CBC:
        strncat(cipherName, "cbc", 4);
        break;
    default:
        return NULL;
    }

    return EVP_get_cipherbyname(cipherName);
}

char * encrypt(const unsigned char *password, const unsigned char *toEncrypt, int cipherAlgo, int mode, int dataLen)
{

    const EVP_CIPHER *selectedEncryptAlgorithm = getCipherAndMode(cipherAlgo, mode);
    int keyLength = EVP_CIPHER_key_length(selectedEncryptAlgorithm);
    int ivLength = EVP_CIPHER_iv_length(selectedEncryptAlgorithm);
    unsigned char * key = malloc(sizeof(unsigned char)*keyLength);
    unsigned char * iv = malloc(sizeof(unsigned char)*ivLength);
    EVP_BytesToKey(selectedEncryptAlgorithm, EVP_sha256(), NULL, password, strlen((char *)password), 1, key, iv);
    char *fileName = "encriptado.txt";
    unsigned char out[MAX_ENCR_LENGTH];
    printf("Key derivada: ");
    showKey(key, keyLength);
    printf("\nIV derivado: ");
    showKey(iv, ivLength);
    int outlen, templ;
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);
    // Parametros para contexto de encripcion
    EVP_EncryptInit_ex(ctx, selectedEncryptAlgorithm, NULL, key, iv);
    // Encripto
    EVP_EncryptUpdate(ctx, out, &outlen, toEncrypt , dataLen);

    EVP_EncryptFinal(ctx, out + outlen, &templ);
    FILE * fp = fopen(fileName, "wb");
    fwrite(out, 1, outlen + templ, fp);
    fclose(fp);
    EVP_CIPHER_CTX_cleanup(ctx);
    free(key);
    free(iv);
    return fileName;
}
