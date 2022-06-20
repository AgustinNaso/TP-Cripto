#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/fileHandler.h"
#include "include/types.h"
#include "include/utils.h"
// #include <openssl/aes.h>


void lsb4(unsigned char msgByte, FILE *input, FILE *output, int groups[4][2])
{
    unsigned char inputFileByte;
    int i = 8;
    fread(&inputFileByte, 1, 1, input);
    for (; i > 4; i--)
    {
        char currBit = GET_NTH_LSB(msgByte, i);
        inputFileByte = modifyBit(inputFileByte, i % 4, currBit);
    }
    fwrite(&inputFileByte, 1, 1, output);
    fread(&inputFileByte, 1, 1, input);
    for (; i > 0; i--)
    {
        char currBit = GET_NTH_LSB(msgByte, i);
        inputFileByte = modifyBit(inputFileByte, i, currBit);
    }
    fwrite(&inputFileByte, 1, 1, output);
}

void lsb1(unsigned char msgByte, FILE *input, FILE *output, int groups[4][2])
{
    unsigned char inputFileByte;
    printf("MsgByte "BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(msgByte));
    for (int i = 8; i > 0; i--)
    {
        fread(&inputFileByte, 1, 1, input);
        char currBit = GET_NTH_LSB(msgByte, i);
        printf("bit %d\n", currBit);
        // Insert currBit in least significant bit of inputFileByte
        printf("fileByte  "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(inputFileByte));
        inputFileByte = modifyBit(inputFileByte, 0, currBit);
        printf("fileByte modified "BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(inputFileByte));
        fwrite(&inputFileByte, 1, 1, output);
    }
}

void lsbiScan(unsigned char msgByte, FILE *input, FILE *output, int groups[4][2])
{
    unsigned char inputFileByte;
    for (int i = 7; i >= 0; i--)
    {
        fread(&inputFileByte, 1, 1, input);
        char currBit = GET_NTH_LSB(msgByte, i);
        if (groups[0][0] != -1)
        {
            char LSB = GET_NTH_LSB(inputFileByte, 1);
            char secondLSB = GET_NTH_LSB(inputFileByte, 2);
            char thirdLSB = GET_NTH_LSB(inputFileByte, 3);
            groups[GET_INT_FROM_2_BITS(thirdLSB, secondLSB)][LSB == currBit]++;
        }
    }
}

void lsbiInsert(unsigned char msgByte, FILE *input, FILE *output, int groups[4][2])
{
    unsigned char inputFileByte;
    for (int i = 7; i >= 0; i--)
    {
        fread(&inputFileByte, 1, 1, input);
        char currBit = GET_NTH_LSB(msgByte, i);
        if (groups[0][0] != -1)
        {
            char LSB = GET_NTH_LSB(inputFileByte, 1);
            char secondLSB = GET_NTH_LSB(inputFileByte, 2);
            char thirdLSB = GET_NTH_LSB(inputFileByte, 3);
            if (groups[GET_INT_FROM_2_BITS(thirdLSB, secondLSB)][MATCHING] < groups[GET_INT_FROM_2_BITS(thirdLSB, secondLSB)][NON_MATCHING])
                currBit = ~currBit;
        }
        inputFileByte = modifyBit(inputFileByte, 0, currBit);
        fwrite(&inputFileByte, 1, 1, output);
    }
}

void encrypt(const char * filePath, int encryptionType, int encryptionMode, const char * pass){
    switch (encryptionType)
    {
    case AES_128:
        break;
    case AES_192:
        break;
    case AES_256:
        break;
    case DES:
        break;
    default:
        break;
    }

}

void embed(const char *bmpPath, const char *filePath, const char *outBmpName, int lsbType , int hasEncryption)
{
    FILE *carrier = fopen(bmpPath, "r");
    FILE *fileToEmbed = fopen(filePath, "r");
    void (*chosenStegAlgorithm)(unsigned char, FILE *, FILE *, int(*)[2]);
    int groups[4][2] = {-1};
    switch (lsbType)
    {
    case LSBI:
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 2; j++)
                groups[i][j] = 0;
        chosenStegAlgorithm = lsbiScan;
        break;
    }
    case LSB1:
        chosenStegAlgorithm = lsb1;
        break;
    case LSB4:
        chosenStegAlgorithm = lsb4;
        break;
    default:
        break;
    }
    const char *fileExtension = getFileExtension(filePath);
    fseek(fileToEmbed, 0L, SEEK_END);
    uint32_t sz = ftell(fileToEmbed);
    rewind(fileToEmbed);
    FILE *output = fopen(outBmpName, "wb");
    bmpFileHeader bfh;
    bmpImageHeader bih;

    fread(&bfh, 1, 14, carrier);
    fread(&bih, 1, sizeof(bih), carrier);
    fwrite(&bfh, 1, 14, output);
    fwrite(&bih, 1, sizeof(bih), output);
    uint32_t headers_position = ftell(output);

    // Embed file size
    unsigned char byte;
    for (char i = 0; i < 4; i++)
    {
        byte = (sz >> (8 * (3 - i))) & 0xff;
        chosenStegAlgorithm(byte, carrier, output, groups);
    }
    // Embed fileToEmbed
    while (fread(&byte, 1, sizeof(byte), fileToEmbed) == 1)
        chosenStegAlgorithm(byte, carrier, output, groups);
    // Embed extension
    for (int i = 0; i < strlen(fileExtension); i++)
        chosenStegAlgorithm(fileExtension[i], carrier, output, groups);
    chosenStegAlgorithm('\0',carrier, output, groups);

    if (lsbType == LSBI)
    {
        fseek(carrier, headers_position, SEEK_SET);
        rewind(fileToEmbed);

        //Inserting which patterns suffered modifications
        for(int i = 0 ; i < 4 ; i++){
            fread(&byte, 1, 1, carrier);
            char groupBit = groups[i][MATCHING] < groups[i][NON_MATCHING];
            byte = modifyBit(byte, 0, groupBit);
            fwrite(&byte, 1, 1, output);
        }

        for (char i = 0; i < 4; i++)
        {
            byte = (sz >> (8 * (3 - i))) & 0xff;
            lsbiInsert(byte, carrier, output, groups);
        }
        // Embed fileToEmbed
        while (fread(&byte, 1, sizeof(byte), fileToEmbed) == 1)
            lsbiInsert(byte, carrier, output, groups);
        // Embed extension
        for (int i = 0; i < strlen(fileExtension); i++)
            lsbiInsert(fileExtension[i], carrier, output, groups);
    }
    // Copy the remaining data
    while (fread(&byte, 1, sizeof(byte), carrier))
        fwrite(&byte, 1, 1, output);
    fclose(fileToEmbed);
    fclose(carrier);
    fclose(output);
    if(hasEncryption == 0)
        remove(filePath);
}

int main(){
    embed("resources/lado.bmp", "archivo1.png","algo.bmp", LSBI, 1);
    return 0;
}