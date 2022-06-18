#include "include/types.h"
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static struct option *longOptions;
// {
//   {"embed", no_argument, NULL, 'b'},
//   {"extract", no_argument, NULL, 'x'},
//   {"in", required_argument, NULL, 'i'},
//   {"p", required_argument, NULL, 'c'},
//   {"out", required_argument, NULL, 'o'},
//   {"steg", required_argument, NULL, 's'},
//   {"a", required_argument, NULL, 'a'},
//   {"m", required_argument, NULL, 'm'},
//   {"pass", required_argument, NULL, 'p'},
//   {NULL, 0, NULL, 0}
// };
static int options = 9;
static int *optionFlags;

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

INPUT_RET parseInput (int argc, char* argv[], InputParams *inputParams) {
  int opt, optIndex = 0;
  struct option currOption;
  optionFlags = calloc(options, sizeof(int));
  initLongOptions();
  inputParams->status = OK;
  opterr = 0;

  while ((opt = getopt_long_only(argc, argv, "", longOptions, &optIndex)) != -1) 
  {
    printf("%d\n", optIndex);
    currOption = longOptions[optIndex];
    opt = currOption.flag == NULL ? currOption.val : *(currOption.flag);
    printf("%d\n", opt);

    switch (opt) {
      case 'b':
      case 'x':
        parseOperation(opt, inputParams);
        break;
      case 'i':
      case 'c':
      case 'o':
      case 's':
      case 'a':
      case 'm':
      case 'p':
        printf("hola %s\n", optarg);
        if (optarg == NULL || optarg[0] == '-') // Consecutive parameters
          inputParams->status = MISSING_ARGUMENT;
        else
          parseParamWithArg(opt, inputParams);
        break;
      default:
        if (optarg == NULL || optarg[0] == '-') {
          printf("hola\n");
          return MISSING_ARGUMENT;
        }
        break;
    }
    
    if (inputParams->status != OK) return inputParams->status;
  }

  checkMissingParams(inputParams);
  free(optionFlags);
  return inputParams->status;
}

void initLongOptions() {
  longOptions = malloc(sizeof(struct option) * (options + 1));
  addOption(EMB, "embed", no_argument, 'b');
  addOption(EXTR, "extract", no_argument, 'x');
  addOption(IN, "in", required_argument, 'i');
  addOption(CARRIER, "p", required_argument, 'c');
  addOption(OUT, "out", required_argument, 'o');
  addOption(STEGO, "steg", required_argument, 's');
  addOption(ENC, "a", required_argument, 'a');
  addOption(MODE, "m", required_argument, 'm');
  addOption(PASSWORD, "pass", required_argument, 'p');
  addOption(END, NULL, 0, 0);
}

void addOption(OPTIONS opt, const char *name, int hasArg, int val) {
  struct option *currOpt = longOptions + opt;

  if (opt == END) {
    currOpt->name = name;
    currOpt->flag = NULL;
  } else {
    currOpt->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy((char*) currOpt->name, name);
    // If flag is not a null pointer, it should be the address of an int variable which is the flag for this option. 
    // The value in val is the value to store in the flag to indicate that the option was seen.
    currOpt->flag = optionFlags + opt;
  }

  currOpt->has_arg = hasArg;
  currOpt->val = val;
}

void parseOperation(int opt, InputParams *inputParams) {
  if (opt == 'b') {
    if (optionFlags[EXTR] != 0) {
      inputParams->status = OPERATION_ERROR;
      return;
    }
    inputParams->operation = EMBED;
  } else {
    if (optionFlags[EMB] != 0) {
      inputParams->status = OPERATION_ERROR;
      return;
    }
    inputParams->operation = EXTRACT;
  }
  return;
}

void parseParamWithArg(int opt, InputParams *inputParams)
{
  switch (opt)
  {
  case 'i':
    if (inputParams->operation == EXTRACT) {
      inputParams->status = PARAMETER_NOT_ALLOWED;
      return;
    }
    inputParams->inputFile = calloc(strlen(optarg) + 1, sizeof(char));
    strcpy(inputParams->inputFile, optarg);
    break;
  case 'c':

    if (parseBMPFile(optarg, inputParams) == BMP_ERROR) return;
    inputParams->carrierFile = calloc(strlen(optarg) + 1, sizeof(char));
    strcpy(inputParams->carrierFile, optarg);
    break;
  case 'o':
    if (inputParams->operation == EMBED && parseBMPFile(optarg, inputParams) != BMP_ERROR) return;
    inputParams->outputFile = calloc(strlen(optarg) + 1, sizeof(char));
    strcpy(inputParams->outputFile, optarg);
    break;
  case 's':
    checkStego(optarg, inputParams);
    break;
  case 'a':
    checkEncryption(optarg, inputParams);
    break;
  case 'm':
    checkMode(optarg, inputParams);
    break;
  case 'p':
    inputParams->password = calloc(strlen(optarg) + 1, sizeof(char));
    memcpy(inputParams->password, optarg, strlen(optarg));
    break;
  default:
    break;
  }
}

BMP_CHECK parseBMPFile(const char *file, InputParams *inputParams) {
  const char *bmp = ".bmp";

  if (strlen(file) <= strlen(bmp) && strcmp(file + strlen(file) - strlen(bmp), bmp) != 0) {
    inputParams->status = NOT_BMP_FILE;
    return BMP_ERROR;
  }
  
  return BMP_OK;
}

void checkStego(const char *stego, InputParams *inputParams) {
  if (strcmp(stego, "LSB1") == 0)
    inputParams->stego = LSB1;
  else if (strcmp(stego, "LSB4") == 0)
    inputParams->stego = LSB4;
  else if (strcmp(stego, "LSBI") == 0)
    inputParams->stego = LSBI;
  else
    inputParams->status = STEGO_NOT_ALLOWED;
}

void checkEncryption(const char *encryption, InputParams *inputParams) {
  if (strcmp(encryption, "aes128") == 0)
    inputParams->encryption = AES_128;
  else if (strcmp(encryption, "aes192") == 0)
    inputParams->encryption = AES_192;
  else if (strcmp(encryption, "aes256") == 0)
    inputParams->encryption = AES_256;
  else if (strcmp(encryption, "des") == 0)
    inputParams->encryption = DES;
  else
    inputParams->status = ENCRYPTION_NOT_ALLOWED;
}

void checkMode(const char *mode, InputParams *inputParams) {
  if (strcmp(mode, "ecb") == 0) 
    inputParams->mode = ECB;
  else if (strcmp(mode, "cfb") == 0) 
    inputParams->mode = CFB;
  else if (strcmp(mode, "ofb") == 0) 
    inputParams->mode = OFB;
  else if (strcmp(mode, "cbc") == 0) 
    inputParams->mode = CBC;
  else
    inputParams->status = MODE_NOT_ALLOWED;
}

void checkMissingParams(InputParams *inputParams) {
  if (optionFlags[EMB] == 0 && optionFlags[EXTR] == 0) {
    inputParams->status = MISSING_OPERATION;
    return;
  }
  if (missingExtractAndEmbedParams() || missingOptionalParams())
    inputParams->status = MISSING_PARAMETER;
}

int missingExtractAndEmbedParams() {
  int embedParams = optionFlags[IN] && optionFlags[CARRIER] && optionFlags[OUT] && optionFlags[STEGO];
  int extractParams = optionFlags[CARRIER] && optionFlags[OUT] && optionFlags[STEGO];
  return (optionFlags[EMB] && !embedParams) || (optionFlags[EXTR] && !extractParams);
}

int missingOptionalParams() {
  int encryption = optionFlags[ENC] && (optionFlags[MODE] == 0 || optionFlags[PASSWORD] == 0);
  int mode = optionFlags[MODE] && (optionFlags[ENC] == 0 || optionFlags[PASSWORD] == 0);
  int pass = optionFlags[PASSWORD] && (optionFlags[ENC] == 0 || optionFlags[MODE] == 0);
  return encryption || mode || pass;
}