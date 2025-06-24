#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TRIGGER/trigger.h"

static int loggingLevel = LOG_INFO;

void free_text_data(void* data)
{
  free(data);
}

//Read data from a text file into a buffer
char* read_text_file(const char* filepath, unsigned int* read_len)
{
  char* text = NULL;
  if (filepath == NULL || filepath[0] == '\0') {
    trigger_log(LOG_WARN, "FILEIO -> Invalid name filepath");
    return text;
  }
  FILE* file = fopen(filepath, "rb");
  if (file == NULL) {
    trigger_log(LOG_WARN, "FILEIO -> [%s] File failed to open", filepath);
    return text;
  }

  fseek(file, 0, SEEK_END);
  unsigned int count = (unsigned int)ftell(file);
  fseek(file, 0, SEEK_SET);
  if(count <= 0) {
    trigger_log(LOG_INFO, "FILEIO -> [%s] File empty", filepath);
    *read_len = 0;
    return text;
  }

  text = (char*)malloc((count+1)*sizeof(char));
  if (text == NULL) {
    trigger_log(LOG_WARN, "FILEIO -> [%s] Failed to allocate memory into buffer", filepath);
    return text;
  }

  *read_len = (unsigned int)fread(text, sizeof(char), count, file);
  text[*read_len] = '\0';
  fclose(file);
  trigger_log(LOG_INFO, "FILEIO -> [%s] File loaded", filepath);
  if (count != *read_len) {
    trigger_log(LOG_DEBUG, "FILEIO -> [%s] Fread count: %d out of Ftell count: %d", filepath, *read_len, count);
  }
  return text;
}

//Call a log message
void trigger_log(const int log_level, const char* fmt, ...)
{
  if (log_level < loggingLevel) return;
  va_list args;
  va_start(args, fmt);

  switch (log_level) {
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
  if (log_level == LOG_FATAL) exit(EXIT_FAILURE);
}

//Set threshold level to display log calls
void set_trigger_log_level(const int log_level)
{
  loggingLevel = log_level;
}
