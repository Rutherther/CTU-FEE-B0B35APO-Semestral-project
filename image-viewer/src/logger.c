#include "logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

logger_t logger_create(LogLevel minimalLevel, FILE *debugFile, FILE *infoFile, FILE *warningFile,
                     FILE *errorFile, logger_t *childLogger) {
  logger_t logger = { debugFile, infoFile, warningFile, errorFile, minimalLevel, childLogger };
  return logger;
}

void print_time(FILE *file) {
  time_t now;
  time(&now);
  struct tm *local = localtime(&now);

  fprintf(file, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
}

void logger_log_vaargs(logger_t *logger, LogLevel level, const char *file_name,
                       const char *function, int line,
                       const char *const message, va_list args) {
  if (logger->childLogger != NULL) {
    va_list copy;
    va_copy(copy, args);
    logger_log_vaargs(logger->childLogger, level, file_name, function, line, message, copy);
    va_end(copy);
  }

  if (level < logger->minimalLevel) {
    return;
  }

  FILE *file = NULL;
  const char *prefix;

  switch (level) {
  case LOG_DEBUG:
    file = logger->debugFile;
    prefix = DEBUG_PREFIX;
    break;
  case LOG_INFO:
    file = logger->infoFile;
    prefix = INFO_PREFIX;
    break;
  case LOG_WARN:
    file = logger->warningFile;
    prefix = WARN_PREFIX;
    break;
  case LOG_ERROR:
    file = logger->errorFile;
    prefix = ERROR_PREFIX;
    break;
  }

  assert(file != NULL);
  fprintf(file, "[");
  print_time(file);
  fprintf(file, "] %s: ", prefix);
  vfprintf(file, message, args);
  fprintf(file, "(%s/%s:%s)", file_name, function, line);
  fprintf(file, "\r\n");
}

void logger_log(logger_t *logger, LogLevel level, const char *file,
                const char *function, int line,
                const char *const message, ...) {
  va_list args;
  va_start(args, message);
  logger_log_vaargs(logger, level, file, function, line, message, args);
  va_end(args);
}

void logger_debug(logger_t *logger, const char *file, const char *function,
                  int line, const char *const message, ...) {
  va_list args;
  va_start(args, message);
  logger_log_vaargs(logger, LOG_DEBUG, file, function, line, message, args);
  va_end(args);
}

void logger_info(logger_t *logger, const char *file, const char *function,
                 int line, const char *const message, ...) {
  va_list args;
  va_start(args, message);
  logger_log_vaargs(logger, LOG_INFO, file, function, line, message, args);
  va_end(args);
}
void logger_warn(logger_t *logger, const char *file, const char *function,
                 int line, const char *const message, ...) {
  va_list args;
  va_start(args, message);
  logger_log_vaargs(logger, LOG_WARN, file, function, line, message, args);
  va_end(args);
}

void logger_error(logger_t *logger, const char *file, const char *function,
                  int line, const char *const message, ...) {
  va_list args;
  va_start(args, message);
  logger_log_vaargs(logger, LOG_ERROR, file, function, line, message, args);
  va_end(args);
}
