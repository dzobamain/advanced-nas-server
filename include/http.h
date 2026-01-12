/* http.h */
#ifndef HTTP_H
#define HTTP_H

#include "core.h"

void handle_client(int client_fd, struct buffer *buf);

void handle_login(int client_fd, struct buffer *buf);
void handle_storage(int client_fd, struct buffer *buf);

void serve_file(int client_fd, struct buffer *buf, const char *filename);
void serve_static_file(int client_fd, struct buffer *buf, const char *path);

#endif /* HTTP_H */
