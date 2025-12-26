#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

extern const char HTTP_404_NOT_FOUND_RESPONSE[];

/* Logging macros with file and line info */
#define LOG_INFO(msg) (printf("[INFO][%s:%d]: %s\n", __FILE__, __LINE__, msg))
#define LOG_WARNING(msg) (printf("[WARNING][%s:%d]: %s\n", __FILE__, __LINE__, msg))
#define LOG_DEBUG(msg) (printf("[DEBUG][%s:%d]: %s\n", __FILE__, __LINE__, msg))
#define LOG_ERROR(msg) (printf("[ERROR][%s:%d]: %s\n", __FILE__, __LINE__, msg))
#define LOG_FATAL(msg) (printf("[FATAL][%s:%d]: %s\n", __FILE__, __LINE__, msg))

/* Server operation macros with file and line info */
#define SER_USER(msg) (printf("[User][%s:%d]: %s\n", __FILE__, __LINE__, msg))
#define SER_ADMIN(msg) (printf("[Admin][%s:%d]: %s\n", __FILE__, __LINE__, msg))
#define SER_SYSTEM(msg) (printf("[System][%s:%d]: %s\n", __FILE__, __LINE__, msg))

#endif // LOGGER_H
