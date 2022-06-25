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
#define MAX_ENCR_LENGTH 1024
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

void showKey(unsigned char key[])
{
    int i;
    for (i = 0; i < 16; i++)
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
        strncat(cipherName, "cfb8", 5);
        break;
    case OFB:
        strncat(cipherName, "ofb8", 5);
        break;
    case CBC:
        strncat(cipherName, "cbc", 4);
        break;
    default:
        return NULL;
    }

    return EVP_get_cipherbyname(cipherName);
}

char * encrypt(const unsigned char *password, const unsigned char *toEncrypt, int cipherAlgo, int mode)
{
    const EVP_CIPHER *selectedEncryptAlgorithm = getCipherAndMode(cipherAlgo, mode);
    unsigned char key[KEY_LENGTH];
    unsigned char * iv = malloc(sizeof(char)*KEY_LENGTH);
    printf("Clave : %d bytes.\n", EVP_CIPHER_key_length(EVP_aes_128_cbc()));
    printf("IV : %d bytes.\n", EVP_CIPHER_iv_length(EVP_aes_128_cbc()));
    EVP_BytesToKey(EVP_aes_128_cbc(), EVP_md5(), NULL, password, strlen((char *)password), 1, key, iv);
    char *fileName = "encriptado.txt";
    unsigned char out[MAX_ENCR_LENGTH];
    printf("Key derivada: ");
    showKey(key);
    printf("\nIV derivado: ");
    showKey(iv);
    int outlen, templ, inl;
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);
    // Parametros para contexto de encripcion
    if(mode == ECB) {iv = NULL;}
    printf("\nadd %p", iv);
    EVP_EncryptInit_ex(ctx, selectedEncryptAlgorithm, NULL, key, iv);
    inl = strlen((char *)toEncrypt);
    // Encripto
    EVP_EncryptUpdate(ctx, out, &outlen, toEncrypt, inl);
    printf("\nencriptadoas %d bytes\n", outlen);
    EVP_EncryptFinal(ctx, out + outlen, &templ);
    printf("encriptados ultimos %d bytes\n", templ);
    saveEncryptedData(out, outlen + templ, fileName);
    EVP_CIPHER_CTX_cleanup(ctx);
    return fileName;
}

// int main()
// {
//     encrypt("testing", "textoocultomensaje", AES_128, ECB);
//     return 0;
// }