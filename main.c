#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/parser.h"
#include "include/fileHandler.h"
#include "include/extract.h"
#include "include/types.h"

static InputParams inputParams;

int main(int argc, char* argv[]) 
{
    INPUT_RET inputRet = parseInput(argc, argv, &inputParams);
    switch (inputRet)
    {
    case OPERATION_ERROR:
        printf("Operation error\n");
        break;
    case MISSING_PARAMETER:
        printf("Error: missing parameter\n");
        break;
    case MISSING_ARGUMENT:
        printf("Error: missing argument\n");
        break;
    case MISSING_OPERATION:
        printf("Error: missing operation\n");
        break;
    case NOT_BMP_FILE:
        printf("Error: required bmp file\n");
        break;
    case PARAMETER_NOT_ALLOWED:
        printf("Error: -in not allowed for this operation\n");
        break;
    case STEGO_NOT_ALLOWED:
        printf("Error: -steg parameter not allowed\n");
        break;
    case ENCRYPTION_NOT_ALLOWED:
        printf("Error: -a parameter not allowed\n");
        break;
    case MODE_NOT_ALLOWED:
        printf("Error: -m parameter not allowed\n");
        break;
    default:
        break;
    }

    if (inputRet == OK) 
    {
        bmpFile *bmp;
        file *extractedFile = malloc(sizeof(file));

        if (inputParams.operation == EXTRACT) 
        {
            if ((bmp = parseBmpFile(inputParams.carrierFile)) == NULL) return EXIT_FAILURE;

            if (extract(bmp, inputParams, extractedFile) == FAILURE) 
            {
                perror("Unable to extract file\n");
                return EXIT_FAILURE;
            }
            
            writeMessageToOutput(extractedFile, inputParams.outputFile);

        }
    }

    return EXIT_SUCCESS;
}