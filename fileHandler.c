#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/fileHandler.h"
#include "include/utils.h"

bmpFile *parseBmpFile(char *bmpPath)
{
    FILE *fd;

    if ((fd = fopen("out.bmp", "r")) == NULL)
    {
        fprintf(stderr, "Error: unable to open BMP file \"%s\"\n", bmpPath);
        return NULL;
    }

    bmpFile *bmp = malloc(sizeof(bmpFile));
    bmp->header = malloc(sizeof(bmpFileHeader));
    bmp->imageHeader = malloc(sizeof(bmpImageHeader));

    if (fread(bmp->header, sizeof(bmpFileHeader), 1, fd) != 1)
    {
        perror("Unable to read BMP header");
        // TODO: free all
        return NULL;
    }

    if (fread(bmp->imageHeader, sizeof(bmpImageHeader), 1, fd) != 1)
    {
        perror("Unable to read BMP image header");
        // TODO: free all
        return NULL;
    }

    if (bmp->imageHeader->bitCount != BITS_PER_PIXEL)
    {
        fprintf(stderr, "Error: expected bits per pixel is %d\n", BITS_PER_PIXEL);
        // TODO: free all
        return NULL;
    }

    bmp->data = malloc(bmp->imageHeader->imageSize);

    if (fread(bmp->data, bmp->imageHeader->imageSize, 1, fd) != 1)
    {
        perror("Unable to read BMP image data");
        // TODO: free all
        return NULL;
    }

    return bmp;
}

void writeMessageToOutput(file *extractedFile, const char *outputFileName)
{
    char *fullOutputFileName = malloc(strlen(outputFileName) + strlen((char *)extractedFile->extension));
    fullOutputFileName = strcpy(fullOutputFileName, outputFileName);
    fullOutputFileName = strcat(fullOutputFileName, (char *)extractedFile->extension);

    FILE *fd = fopen(fullOutputFileName, "w+"); // reading necessary?
    fwrite(extractedFile->data, extractedFile->size, 1, fd);
    fclose(fd);

    free(fullOutputFileName);
}

void copyBmpData(FILE *file, rgbData *pixels, int img_size)
{
    for (int i = 0; i < img_size; i++)
    {
        rgbData BGR = pixels[i];
        unsigned char color[3] = {BGR.b, BGR.g, BGR.r};
        fwrite(color, 1, sizeof(color), file);
    }
}

void initFileHeader(bmpFileHeader *bfh, int file_size)
{
    memcpy(&(bfh->type), "BM", 2);
    bfh->size = file_size;
    bfh->reserved1 = 0;
    bfh->reserved2 = 0;
    bfh->offset = 0;
}

void initImageHeader(bmpImageHeader *bih, int ppm, int file_size, int width, int height)
{
    bih->headerSize = sizeof(*bih);
    bih->width = width;
    bih->height = height;
    bih->planes = 1;
    bih->bitCount = 24;
    bih->compression = 0;
    bih->imageSize = file_size;
    bih->ppmX = ppm;
    bih->ppmY = ppm;
    bih->colorsUsed = 0;
    bih->importantColors = 0;
}

void createBmpFile(char *file_name, int height, int width, rgbData *pixels)
{
    bmpImageHeader bih;
    bmpFileHeader bfh;
    // dpi a ppm https://www.wikiwand.com/en/Dots_per_inch
    int ppm = 96 * 39.375;
    int img_size = height * width;
    int file_size = HEADER_SIZE + 8 * img_size;
    initFileHeader(&bfh, file_size);
    initImageHeader(&bih, ppm, file_size, width, height);
    printf("%d\n", bih.headerSize);

    FILE *fp = fopen(file_name, "wb");
    fwrite(&bfh, 1, 14, fp);
    fwrite(&bih, 1, sizeof(bih), fp);
    copyBmpData(fp, pixels, img_size);

    return;
}

void lsb4(unsigned char msgByte, FILE *input, FILE *output, int groups[4][2])
{
    unsigned char inputFileByte;
    int i = 7;
    fread(&inputFileByte, 1, 1, input);
    for (; i >= 4; i--)
    {
        char currBit = ((msgByte) >> i) & 1;
        inputFileByte = modifyBit(inputFileByte, i % 4, currBit);
    }
    fwrite(&inputFileByte, 1, 1, output);
    fread(&inputFileByte, 1, 1, input);
    for (; i >= 0; i--)
    {
        char currBit = ((msgByte) >> i) & 1;
        inputFileByte = modifyBit(inputFileByte, i, currBit);
    }
    fwrite(&inputFileByte, 1, 1, output);
}

void lsb1(unsigned char msgByte, FILE *input, FILE *output, int groups[4][2])
{
    unsigned char inputFileByte;
    for (int i = 7; i >= 0; i--)
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

void embed(const char *bmpPath, const char *filePath, const char *outBmpName, int lsbType)
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

    if (lsbType == LSBI)
    {
        fseek(carrier, headers_position, SEEK_SET);
        rewind(fileToEmbed);

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
        //Inserting which patterns suffered modifications
        for(int i = 0 ; i < 4 ; i++){
            fread(&byte, 1, 1, carrier);
            char groupBit = groups[i][MATCHING] < groups[i][NON_MATCHING];
            byte = modifyBit(byte, 0, groupBit);
            fwrite(&byte, 1, 1, output);
        }
    }
    // Copy the remaining data
    while (fread(&byte, 1, sizeof(byte), carrier))
        fwrite(&byte, 1, 1, output);
    fclose(fileToEmbed);
    fclose(carrier);
    fclose(output);
}

int main(int argc, char *argv[])
{
    embed("resources/lado.bmp", "msg.txt", "out.bmp", LSBI);
    // FILE *input = fopen("resources/ladoLSB1.bmp", "r");
    // FILE *msg = fopen("msg.txt", "r");
    // const char * fileExtension = getFileExtension("msg.txt");
    // fseek(msg, 0L, SEEK_END);
    // uint32_t sz = ftell(msg);
    // rewind(msg);
    // printf("size: %d\n", sz);
    // FILE *output = fopen("out.bmp", "wb");
    // bmpFileHeader bfh;
    // bmpImageHeader bih;

    // fread(&bfh, 1, 14, input);
    // fread(&bih, 1, sizeof(bih), input);
    // fwrite(&bfh, 1, 14, output);
    // fwrite(&bih, 1, sizeof(bih), output);

    // //Embed file size
    // char szBits[32];
    // for (char i = 0; i < 32; i++)
    // {
    //     szBits[i] = (sz >> 32 - 1 - i) & 1;
    // }
    // unsigned char byte;
    // for (char i = 0; i < 32; i++)
    // {
    //     fread(&byte, 1, 1, input);
    //     byte = (byte & ~1) | szBits[i];
    //     fwrite(&byte, 1, 1, output);
    // }
    // //Embed msg
    // while(fread(&byte, 1, sizeof(byte), msg) == 1)
    //     lsb1(byte, input, output);
    // //Embed extension
    // for(int i = 0; i < strlen(fileExtension); i++)
    //     lsb1(fileExtension[i], input, output);
    // //Copy the remaining data
    // while(fread(&byte, 1, sizeof(byte), input))
    //     fwrite(&byte, 1, 1, output);

    return 0;
}