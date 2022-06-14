#include "include/utils.h"
#include <string.h>

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

int modifyBit(int number, int position, int bit)
{
    int mask = 1 << position;
    return ((number & ~mask) | (bit << position));
}