#include <stdio.h>
#include <stdlib.h>
#include <openssl/des.h>
#include <openssl/bio.h>
#include <string.h>
#include <openssl/evp.h>

#define BLOCK_SIZE 8
#define SUCCESS 0
#define FAILURE 1

int Encrypt(unsigned char *in, int inl, unsigned char *out, DES_key_schedule *ks);
unsigned char *padding(unsigned char *in, int *inl, size_t blocksize);
int saveEncryptedData(unsigned char *out, int len, unsigned char *where);

unsigned char *padding(unsigned char *in, int *inl, size_t blocksize)
{
    int pad;
    int i;
    unsigned char *inPad;
    pad = blocksize - (*inl) % blocksize;
    inPad = malloc(*inl + pad);
    memcpy(inPad, in, *inl);
    for (i = (*inl); i < (*inl + pad); i++)
        inPad[i] = pad;
    *inl += pad;
    return (inPad);
}

int saveEncryptedData(unsigned char *out, int len, unsigned char *where)
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

int Encrypt(unsigned char *in, int inl, unsigned char *out, DES_key_schedule *ks)
{
    DES_cblock inB;
    DES_cblock outB;
    int numB;
    int i;
    numB = inl / BLOCK_SIZE;
    for (i = 0; i < numB; i++)
    {
        memcpy(inB, in + i * BLOCK_SIZE, BLOCK_SIZE);
        DES_ecb_encrypt(&inB, &outB, ks, DES_ENCRYPT);
        memcpy(out + i * BLOCK_SIZE, outB, BLOCK_SIZE);
    }
    return 1;
}

int des_enc(const char * filePath, int mode)
{

    FILE *fileptr;
    unsigned char *buffer;
    long filelen;

    fileptr = fopen(filePath, "rb");  // Open the file in binary mode
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(fileptr);             // Get the current byte offset in the file
    rewind(fileptr);                      // Jump back to the beginning of the file

    buffer = (char *)malloc(filelen * sizeof(char)); // Enough memory for the file
    fread(buffer, filelen, 1, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file

    unsigned char *nomArch = "des-ecb.txt";
    unsigned char *inPad;
    unsigned char *out;
    int inl, outl;
    DES_cblock k = "1";
    DES_key_schedule ks;
    /* Setear paridad impar*/
    DES_set_odd_parity(&k);
    /* Setear key schedule*/
    DES_set_key_checked(&k, &ks);
    /* Completar con padding*/
    inl = strlen(buffer);
    inPad = padding(buffer, &inl, BLOCK_SIZE);
    /* Modo ecb encripta de a BLOQUES*/
    out = malloc(inl);
    outl = inl;
    Encrypt(inPad, inl, out, &ks);
    saveEncryptedData(out, outl, nomArch);
    free(inPad);
    free(out);
    return EXIT_SUCCESS;
}

int main(){
    des_enc("msg.txt",1);
    return 0;
}