#ifndef PARSER_H
#define PARSER_H

#include "types.h"

INPUT_RET parseInput (int argc, char* argv[], InputParams *inputParams);
void initLongOptions();
void addOption(OPTIONS opt, const char *name, int argOpt, int val);
void parseOperation(int opt, InputParams *inputParams);
void parseParamWithArg(int opt, InputParams *inputParams);
BMP_CHECK parseBMPFile(const char *file, InputParams *inputParams);
void checkStego(const char *stego, InputParams *inputParams);
void checkEncryption(const char *encryption, InputParams *inputParams);
void checkMode(const char *mode, InputParams *inputParams);
void checkMissingParams(InputParams *inputParams);
int missingExtractAndEmbedParams();
int missingOptionalParams();


#endif