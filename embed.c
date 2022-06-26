#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/fileHandler.h"
#include "include/types.h"
#include "include/encrypt.h"
#include "include/utils.h"
// #include <openssl/aes.h>

void lsb4(unsigned char msgByte, FILE *input, FILE *output, int groups[4][2])
{
    unsigned char inputFileByte;
    int i = 8;
    fread(&inputFileByte, 1, 1, input);
    for (; i > 4; i--)
    {
        printf("\n " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(inputFileByte));
        char currBit = GET_NTH_LSB(msgByte, i);
        printf("\n currBit: %d", currBit);
        inputFileByte = modifyBit(inputFileByte, i -1 - 4 , currBit);
        printf("\n "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(inputFileByte));
    }
    fwrite(&inputFileByte, 1, 1, output);
    fread(&inputFileByte, 1, 1, input);
    for (; i > 0; i--)
    {
        printf("\n 2da mitad" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(inputFileByte));
        char currBit = GET_NTH_LSB(msgByte, i);
        printf("\n currBit: %d", currBit);
        inputFileByte = modifyBit(inputFileByte,  i - 1, currBit);
        printf("\n 2da mitad" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(inputFileByte));
    }
    fwrite(&inputFileByte, 1, 1, output);
}

void lsb1(unsigned char msgByte, FILE *input, FILE *output, int groups[4][2])
{
    unsigned char inputFileByte;
    for (int i = 8; i > 0; i--)
    {
        fread(&inputFileByte, 1, 1, input);
        char currBit = GET_NTH_LSB(msgByte, i);
        // Insert currBit in least significant bit of inputFileByte
        inputFileByte = modifyBit(inputFileByte, 0, currBit);
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

void lsbiInsert(unsigned char msgByte, FILE *carrier, FILE *output, int groups[4][2])
{
    unsigned char inputFileByte;
    for (int i = 8; i > 0; i--)
    {
        fread(&inputFileByte, 1, 1, carrier);
        char currBit = GET_NTH_LSB(msgByte, i);
        if (groups[0][0] != -1)
        {
            // char LSB = GET_NTH_LSB(inputFileByte, 1);
            char secondLSB = GET_NTH_LSB(inputFileByte, 2);
            char thirdLSB = GET_NTH_LSB(inputFileByte, 3);
            if (groups[GET_INT_FROM_2_BITS(thirdLSB, secondLSB)][MATCHING] < groups[GET_INT_FROM_2_BITS(thirdLSB, secondLSB)][NON_MATCHING])
                currBit = ~currBit & 1u;
        }
        inputFileByte = modifyBit(inputFileByte, 0, currBit);
        fwrite(&inputFileByte, 1, 1, output);
    }
}


void embed(const char *bmpPath, const char *filePath, const char *outBmpName, int lsbType, int hasEncryption)
{
    FILE *carrier = fopen(bmpPath, "r");
    FILE *fileToEmbed = fopen(filePath, "r");
    void (*chosenStegAlgorithm)(unsigned char, FILE *, FILE *, int(*)[2]);
    int groups[4][2] = {{-1}};
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
    printf("%s\n",outBmpName);
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
    if (lsbType == LSBI)
    {
        fseek(carrier, headers_position + 4, SEEK_SET);
    }
    for (char i = 0; i < 4; i++)
    {
        byte = (sz >> (8 * (3 - i))) & 0xff;
        chosenStegAlgorithm(byte, carrier, output, groups);
    }
    // Embed fileToEmbed
    while (fread(&byte, 1, sizeof(byte), fileToEmbed) == 1)
        chosenStegAlgorithm(byte, carrier, output, groups);
    // Embed extension
    if (hasEncryption == 0)
    {
        for (int i = 0; i < strlen(fileExtension); i++)
            chosenStegAlgorithm(fileExtension[i], carrier, output, groups);
    }
    chosenStegAlgorithm('\0', carrier, output, groups);

    if (lsbType == LSBI)
    {
        fseek(carrier, headers_position, SEEK_SET);
        rewind(fileToEmbed);

        // Inserting which patterns suffered modifications
        for (int i = 0; i < 4; i++)
        {
            fread(&byte, 1, 1, carrier);
            char groupBit = groups[i][MATCHING] < groups[i][NON_MATCHING];
            printf("%d\n", groupBit);
            printf("%d\n", groups[i][MATCHING]);
            printf("%d\n----------\n", groups[i][NON_MATCHING]);
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
        if (hasEncryption == 0)
        {
            for (int i = 0; i < strlen(fileExtension); i++)
                lsbiInsert(fileExtension[i], carrier, output, groups);
        }
        lsbiInsert('\0', carrier, output, groups);

    }
    // Copy the remaining data
    while (fread(&byte, 1, sizeof(byte), carrier))
        fwrite(&byte, 1, 1, output);
    fclose(fileToEmbed);
    fclose(carrier);
    fclose(output);
    if (hasEncryption)
        remove(filePath);
}

int checkCarrierSize(char * fileToEmbed, char * carrierPath, int stegMode){
    FILE * fp = fopen(fileToEmbed, "r+");
    fseek(fp, 0L, SEEK_END);
    uint32_t fileToEmbedSize = ftell(fp);
    fileToEmbedSize+= strlen(getFileExtension(fileToEmbed));
    fclose(fp)
    fp = fopen(carrierPath, "r+");
    fseek(fp, 0L, SEEK_END);
    uint32_t carrierSize = ftell(fp);
    fclose(fp)
    int sizeDifference =-1;
    switch (stegMode)
    {
    case LSB1:
        sizeDifference  = carrierSize - fileToEmbedSize * 8;
        break;
    case LSB4:
        sizeDifference  = carrierSize - fileToEmbedSize * 4;
        break;
    case LSBI:
        sizeDifference  = carrierSize - (fileToEmbedSize * 8 + 4)
    break;
    default:
        break;
    }
    return sizeDifference;
}

int handleEmbedding(char * fileToEmbed, char * carrierPath, char * embeddedFileName, int embedMode, int encryptAlgo, int encryptMode, unsigned char * password)
{
    if(checkCarrierSize(fileToEmbed, carrierPath, embed) < 0){
        printf("File to embed doesn't fit in carrier\n");
        return -1;
    }
    FILE * fp = fopen(fileToEmbed, "r+");
    fseek(fp, 0L, SEEK_END);
    uint32_t sz = ftell(fp);
    printf(" file size %d\n", sz);
    rewind(fp);
    const char * extension = getFileExtension(fileToEmbed);
    printf("ext %s %ld\n", extension, strlen(extension));
    int dataLen = sz + 4 + strlen(extension) + 1;
    unsigned char * data = malloc(sizeof(char) * (dataLen));
    sizeTo4ByteArray(sz, data);
    fread(data + 4, sz, 1, fp);
    printf("char %d %d %d %d\n", data[0], data[1], data[2], data[3]);
    for(int i = 0 ; i < strlen(extension); i++)
        data[sz + 4 + i] = extension[i];
    char * filePath = fileToEmbed;
    int encrypted = 1;
    if(encryptMode == 0) encrypted = 0;
    printf("\n");
    for(int i = 4 ; i< sz + 4 + strlen(extension); i++)
        printf("%c", data[i]);
    data[sz + 4 + strlen(extension)] = 0;
    printf("\nenc %d\n", encrypted);
    if(encrypted != 0)
        filePath = encrypt(password, data, encryptAlgo, encryptMode, dataLen);
    embed(carrierPath, filePath, embeddedFileName, embedMode, encrypted);
    fclose(fp);
    free(data);
    return 0;
}
