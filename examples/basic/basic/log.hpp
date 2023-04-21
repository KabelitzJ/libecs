#ifndef LOG_HPP_
#define LOG_HPP_

#include <cstdarg>

struct logger;

typedef struct logger logger_t;

logger_t* logger_create(const char* tag);

void logger_destroy(logger_t* logger);

void logger_debug(const logger_t* logger, const char* format, ...);

void logger_info(const logger_t* logger, const char* format, ...);

void logger_warn(const logger_t* logger, const char* format, ...);

void logger_error(const logger_t* logger, const char* format, ...);

#endif // LOG_HPP_
