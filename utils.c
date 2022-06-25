#include "include/utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const char* getFileExtension(const char * path)
{
    const char *result;
    int i, n;

    n = strlen(path);
    i = n - 1;
    while ((i >= 0) && (path[i] != '.') && (path[i] != '/') & (path[i] != '\\')) {
        i--;
    }
    if ((i > 0) && (path[i] == '.') && (path[i - 1] != '/') && (path[i - 1] != '\\')) {
        result = path + i;
    } else {
        result = path + n;
    }
    return result;
}

int modifyBit(unsigned char byte, int position, int bit)
{
    int mask = 1 << position;
    return ((byte & ~mask) | (bit << position));
}

void errorHandler(const char *errorMessage) {
    fprintf(stderr, "Failed to %s\n", errorMessage);
    exit(0);
}
