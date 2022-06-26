#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/parser.h"
#include "include/fileHandler.h"
#include "include/extract.h"
#include "include/types.h"
#include "include/embed.h"

static InputParams inputParams;

int main(int argc, char* argv[])
{
    INPUT_RET inputRet = parseInput(argc, argv, &inputParams);

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
            freeBmpFile(bmp);
        }
        else if (inputParams.operation == EMBED)
        {
            handleEmbedding(inputParams.inputFile, inputParams.carrierFile, inputParams.outputFile, inputParams.stego, inputParams.encryption, inputParams.mode, inputParams.password);
        }
        
        freeFile(extractedFile);
    }


    return EXIT_SUCCESS;
}