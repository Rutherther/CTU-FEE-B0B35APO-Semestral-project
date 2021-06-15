#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>

#define DEBUG_PREFIX "[DEBG]"
#define INFO_PREFIX  "[INFO]"
#define WARN_PREFIX  "[WARN]"
#define ERROR_PREFIX "[ERRO]"

typedef enum LogLevel {
  LOG_DEBUG = 1,
  LOG_INFO = 2,
  LOG_WARN = 3,
  LOG_ERROR = 4,
} LogLevel;

typedef struct logger_t logger_t;

struct logger_t {
  FILE *debugFile;
  FILE *infoFile;
  FILE *warningFile;
  FILE *errorFile;

  LogLevel minimalLevel;

  logger_t *childLogger; // Duplicate logs to this logger
};

logger_t logger_create(LogLevel minimalLevel, FILE *debugFile, FILE *infoFile, FILE *warningFile,
                     FILE *errorFile, logger_t *childLogger);

void logger_log(logger_t *logger, LogLevel level, const char *file, const char *function, int line, const char *const message, ...);
void logger_debug(logger_t *logger, const char *file, const char *function,
                  int line, const char *const message, ...);
void logger_info(logger_t *logger, const char *file, const char *function,
                 int line, const char *const message, ...);
void logger_warn(logger_t *logger, const char *file, const char *function,
                 int line, const char *const message, ...);
void logger_error(logger_t *logger, const char *file, const char *function,
                  int line, const char *const message, ...);

#endif //__LOGGER_H__
