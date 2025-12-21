#include <stdio.h>

/* Logging macros with file and line info */
#define LOG_INFO(msg) (printf("INFO [%s:%d]: %s\n", __FILE__, __LINE__, msg))
#define LOG_WARNING(msg) (printf("WARNING [%s:%d]: %s\n", __FILE__, __LINE__, msg))
#define LOG_DEBUG(msg) (printf("DEBUG [%s:%d]: %s\n", __FILE__, __LINE__, msg))
#define LOG_ERROR(msg) (printf("ERROR [%s:%d]: %s\n", __FILE__, __LINE__, msg))
#define LOG_FATAL(msg) (printf("FATAL [%s:%d]: %s\n", __FILE__, __LINE__, msg))

/* Server operation macros with file and line info */
#define SER_USER_DO(msg) (printf("User operation [%s:%d]: %s\n", __FILE__, __LINE__, msg))
#define SER_ADMIN_DO(msg) (printf("Admin operation [%s:%d]: %s\n", __FILE__, __LINE__, msg))
#define SER_SYSTEM_DO(msg) (printf("System operation [%s:%d]: %s\n", __FILE__, __LINE__, msg))
