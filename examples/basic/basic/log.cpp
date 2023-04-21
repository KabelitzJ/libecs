#include <basic/log.hpp>

#include <cstdio>
#include <ctime>
#include <cinttypes>
#include <cstring>
#include <cstddef>
#include <cstdlib>

static const uint8_t logger_max_tag_size = 32u; 

struct logger {
  char tag[logger_max_tag_size];
};

enum log_level {
  LOG_LEVEL_DEBUG = 0,
  LOG_LEVEL_INFO = 1,
  LOG_LEVEL_WARN = 2,
  LOG_LEVEL_ERROR = 3,
  LOG_LEVEL_COUNT
};

typedef uint8_t log_level_t;

struct log_payload {
  const char* name;
  const char* color;
};

static log_payload _log_payloads[LOG_LEVEL_COUNT] = {
  {"debug", "1;34"},
  {"info", "1;32"},
  {"warn", "1;33"},
  {"error", "1;31"}
};

static void _log(const logger_t* logger, log_level_t level, const char* format, va_list* args) {
  va_list args_copy;
  va_copy(args_copy, *args);

  int length = vsnprintf(NULL, 0, format, args_copy);

  va_end(args_copy);

  if (length < 0) {
    return;
  }

  char buffer[length + 1];

  length = vsnprintf(buffer, sizeof(buffer), format, *args);

  if (length < 0) {
    return;
  }

  time_t current_time = time(NULL);

  if (current_time == -1) {
    return;
  }

  struct tm* tm = localtime(&current_time);

  if (tm == NULL) {
    return;
  }

  fprintf(stdout, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
  fprintf(stdout, " \033[0m\033[%sm[%s]\033[0m", _log_payloads[level].color, _log_payloads[level].name);
  fprintf(stdout, " [%s]", logger->tag);
  fprintf(stdout, " : %s\n", buffer);
  fflush(stdout);
}

logger_t* logger_create(const char* tag) {
  logger_t* logger = (logger_t*)malloc(sizeof(logger_t));

  if (logger != NULL) {
    memset(logger, 0, sizeof(logger_t));
    strncpy(logger->tag, tag, logger_max_tag_size);
  }

  return logger;
}

void logger_destroy(logger_t* logger) {
  free(logger);
  logger = NULL;
}

void logger_debug(const logger_t* logger, const char* format, ...) {
  va_list args;
  va_start(args, format);
  _log(logger, LOG_LEVEL_DEBUG, format, &args);
  va_end(args);
}

void logger_info(const logger_t* logger, const char* format, ...) {
  va_list args;
  va_start(args, format);
  _log(logger, LOG_LEVEL_INFO, format, &args);
  va_end(args);
}

void logger_warn(const logger_t* logger, const char* format, ...) {
  va_list args;
  va_start(args, format);
  _log(logger, LOG_LEVEL_WARN, format, &args);
  va_end(args);
}

void logger_error(const logger_t* logger, const char* format, ...) {
  va_list args;
  va_start(args, format);
  _log(logger, LOG_LEVEL_ERROR, format, &args);
  va_end(args);
}
