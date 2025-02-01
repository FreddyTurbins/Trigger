#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TRIGGER/trigger.h"

static int loggingLevel = LOG_INFO;

void FreeTextData(void* data)
{
  free(data);
}

//Read data from a text file into a buffer
char* ReadTextFile(const char* filepath, unsigned int* readLen)
{
  char* text = NULL;
  if (filepath == NULL || filepath[0] == '\0') {
    TriggerLogCall(LOG_WARN, "FILEIO -> Invalid name filepath");
    return text;
  }
  FILE* file = fopen(filepath, "rb");
  if (file == NULL) {
    TriggerLogCall(LOG_WARN, "FILEIO -> [%s] File failed to open", filepath);
    return text;
  }

  fseek(file, 0, SEEK_END);
  unsigned int count = (unsigned int)ftell(file);
  fseek(file, 0, SEEK_SET);
  if(count <= 0) {
    TriggerLogCall(LOG_WARN, "FILEIO -> [%s] File failed to read", filepath);
    return text;
  }

  text = (char*)malloc((count+1)*sizeof(char));
  if (text == NULL) {
    TriggerLogCall(LOG_WARN, "FILEIO -> [%s] Failed to allocate memory into buffer", filepath);
    return text;
  }

  *readLen = (unsigned int)fread(text, sizeof(char), count, file);
  text[*readLen] = '\0';
  fclose(file);
  TriggerLogCall(LOG_INFO, "FILEIO -> [%s] File loaded", filepath);
  if (count != *readLen) {
    TriggerLogCall(LOG_DEBUG, "FILEIO -> [%s] Fread count: %d out of Ftell count: %d", filepath, *readLen, count);
  }
  return text;
}

//Call a log message
void TriggerLogCall(const int logLevel, const char* fmt, ...)
{
  if (logLevel < loggingLevel) return;
  va_list args;
  va_start(args, fmt);

  switch (logLevel) {
    case LOG_TRACE: printf("\x1b[94m[TRACE]: ");    break;
    case LOG_DEBUG: printf("\x1b[36m[DEBUG]: ");    break;
    case LOG_INFO:  printf("\x1b[32m[INFO]: ");     break;
    case LOG_WARN:  printf("\x1b[33m[WARNING]: ");  break;
    case LOG_ERROR: printf("\x1b[31m[ERROR]: ");    break;
    case LOG_FATAL: printf("x1b[35m[FATAL]: ");    break;
    default: break;
  };
  printf("\x1b[0m\x1b[90m");
  vfprintf(stdout, fmt, args);
  printf("\x1b[0m\n");
  va_end(args);
  if (logLevel == LOG_FATAL) exit(EXIT_FAILURE);
}

//Set threshold level to display log calls
void SetTriggerLogLevel(const int logLevel)
{
  loggingLevel = logLevel;
}
