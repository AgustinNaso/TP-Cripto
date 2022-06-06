#include <stdio.h>
#include <string.h>
#include "include/parser.h"

static InputParams inputParams;

int main(int argc, char* argv[]) {
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
        break;;
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
    printf("Input ret: %d\n", inputRet);
    return 0;
}