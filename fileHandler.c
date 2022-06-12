#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/fileHandler.h"

bmpFile *parseBmpFile(char *bmpPath) 
{
    FILE *fd;

    if ((fd = fopen(bmpPath, "r")) == NULL) 
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
        //TODO: free all
        return NULL;
    }

    if (fread(bmp->imageHeader, sizeof(bmpImageHeader), 1, fd) != 1) 
    {
        perror("Unable to read BMP image header");
        //TODO: free all
        return NULL;
    }

    if (bmp->imageHeader->bitCount != BITS_PER_PIXEL) 
    {
        fprintf(stderr, "Error: expected bits per pixel is %d\n", BITS_PER_PIXEL);
        //TODO: free all
        return NULL;
    }

    bmp->data = malloc(bmp->imageHeader->imageSize);

    if (fread(bmp->data, bmp->imageHeader->imageSize, 1, fd) != 1) 
    {
        perror("Unable to read BMP image data");
        //TODO: free all
        return NULL;
    }

    return bmp;
}

void writeMessageToOutput(file *extractedFile, const char *outputFileName) 
{
    char *fullOutputFileName = malloc(strlen(outputFileName) + strlen((char *) extractedFile->extension));
    fullOutputFileName = strcpy(fullOutputFileName, outputFileName);
    fullOutputFileName = strcat(fullOutputFileName, (char *) extractedFile->extension);

    FILE *fd = fopen(fullOutputFileName, "w+"); //reading necessary?
    fwrite(extractedFile->data, extractedFile->size, 1, fd);
    fclose(fd);

    free(fullOutputFileName);
}

void copyBmpData(FILE* file, rgbData* pixels, int img_size)
{
    for (int i = 0; i < img_size; i++)
    {
        rgbData BGR = pixels[i];
        unsigned char color[3] = {BGR.b, BGR.g, BGR.r};
        fwrite(color, 1, sizeof(color), file);
    }
}


void initFileHeader(bmpFileHeader* bfh, int file_size)
{
    memcpy(&(bfh->type), "BM", 2);
    bfh->size = file_size;
    bfh->reserved1 = 0;
    bfh->reserved2 = 0;
    bfh->offset = 0;
}

void initImageHeader(bmpImageHeader* bih, int ppm, int file_size, int width, int height)
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


void createBmpFile(char* file_name, int height, int width, rgbData* pixels)
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

void lsb1(unsigned char* pixel, FILE* msg, int * bitsToRead, unsigned char* currChar){
    int charsRead = 0;
    for(int i = 0 ; i < 3 ; i++){
        if(*bitsToRead == 0 ){
            charsRead = fread(currChar, 1, 1, msg);
            if(charsRead == 0) return;
            *bitsToRead = 8;
        }
        char currBit = ((*currChar)>>((*bitsToRead) - 1))&1;
        printf("%d\n", currBit);
        pixel[i] = (pixel[i] & ~1) | currBit;
        (*bitsToRead)--;
    }
}


// int main(int argc, char *argv[])
// {
//     FILE* input = fopen("resources/ladoLSB1.bmp", "r");
//     FILE* msg = fopen("msg.txt","r");
//     FILE* output = fopen("out.bmp","wb");
//     bmpFileHeader bfh;
//     bmpImageHeader bih;

//     fread(&bfh, 1, 14, input);
//     fread(&bih,1, sizeof(bih), input);
//     fwrite(&bfh, 1, 14, output);
//     fwrite(&bih, 1, sizeof(bih), output);


//     unsigned char pixel[3];
//     int bitsToRead = 0;
//     unsigned char currChar;
//     for(int i = 0 ;  i < bih.imageSize/3 ; i++ ){ 
//         fread(pixel ,1, sizeof(pixel), input);
//         lsb1(pixel, msg, &bitsToRead, &currChar);
//         fwrite(pixel,1,sizeof(pixel),output);
//     }

//     return 0;
// }