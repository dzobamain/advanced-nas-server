#include "logger.h"

const char HTTP_404_NOT_FOUND_RESPONSE[] =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/plain\r\n"
    "Connection: close\r\n\r\n"
    "File not found";
