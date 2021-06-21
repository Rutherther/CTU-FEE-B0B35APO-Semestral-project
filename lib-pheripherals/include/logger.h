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

/**
 * @brief Create logger_t
 * 
 * @param minimalLevel minimal level to log, levels below this one will not be logged
 * @param debugFile file to output debug level to
 * @param infoFile file to output input level to
 * @param warningFile file to output warning level to
 * @param errorFile file to output error level to
 * @param childLogger child logger that will be called every log to use for duplicating to another file
 * @return logger_t 
 */
logger_t logger_create(LogLevel minimalLevel, FILE *debugFile, FILE *infoFile, FILE *warningFile,
                     FILE *errorFile, logger_t *childLogger);
/**
 * @brief Log given message
 * 
 * @param logger
 * @param level level of log
 * @param file name of file __FILE__
 * @param function name of function __FUNCTION__
 * @param line number of line __LINE__
 * @param message message with format like printf 
 * @param ... arguments to printf
 */
void logger_log(logger_t *logger, LogLevel level, const char *file, const char *function, int line, const char *const message, ...);

/**
 * @brief Log given debug message
 * 
 * @param logger
 * @param file name of file __FILE__
 * @param function name of function __FUNCTION__
 * @param line number of line __LINE__
 * @param message message with format like printf 
 * @param ... arguments to printf
 */
void logger_debug(logger_t *logger, const char *file, const char *function,
                  int line, const char *const message, ...);

/**
 * @brief Log given info message
 * 
 * @param logger
 * @param file name of file __FILE__
 * @param function name of function __FUNCTION__
 * @param line number of line __LINE__
 * @param message message with format like printf 
 * @param ... arguments to printf
 */
void logger_info(logger_t *logger, const char *file, const char *function,
                 int line, const char *const message, ...);

/**
 * @brief Log given warn message
 * 
 * @param logger
 * @param file name of file __FILE__
 * @param function name of function __FUNCTION__
 * @param line number of line __LINE__
 * @param message message with format like printf 
 * @param ... arguments to printf
 */
void logger_warn(logger_t *logger, const char *file, const char *function,
                 int line, const char *const message, ...);

/**
 * @brief Log given error message
 * 
 * @param logger
 * @param file name of file __FILE__
 * @param function name of function __FUNCTION__
 * @param line number of line __LINE__
 * @param message message with format like printf 
 * @param ... arguments to printf
 */
void logger_error(logger_t *logger, const char *file, const char *function,
                  int line, const char *const message, ...);

#endif //__LOGGER_H__
