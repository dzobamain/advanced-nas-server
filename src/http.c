#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "http.h"
#include "config.h"
#include "logger.h"
#include "db.h"

void handle_client(int client_fd, struct buffer *buf)
{
    ssize_t r = read(client_fd, buf->data, buf->length - 1);
    if (r <= 0) {
        close(client_fd);
        return;
    }
    buf->data[r] = '\0';

    char method[8], path[256];
    sscanf(buf->data, "%7s %255s", method, path);

    if (strcmp(method, "POST") == 0 && strcmp(path, "/login") == 0) {
        handle_login(client_fd, buf);
        return;
    }
    
    serve_static_file(client_fd, buf, path);
}

void handle_login(int client_fd, struct buffer *buf)
{
    char *body = strstr(buf->data, "\r\n\r\n");
    if (!body) {
        close(client_fd);
        return;
    }
    body += 4;

    struct user usr; 
    sscanf(body, "username=%63[^&]&password=%63s", usr.username, usr.password);

    int is_user = is_user_in_db(DATABASE_FILE, &usr);
    if (is_user == 1) {
        const char *ok =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n\r\n"
            "LOGIN_OK";
        write(client_fd, ok, strlen(ok));
    } else {
        const char *fail =
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n\r\n"
            "LOGIN_FAILED";
        write(client_fd, fail, strlen(fail));
    }

    close(client_fd);
}


void serve_static_file(int client_fd, struct buffer *buf, const char *path)
{
    char filename[256];

    if (strcmp(path, "/") == 0)
        snprintf(filename, sizeof(filename), "%s", LOGIN_HTML_FILE);
    else
        snprintf(filename, sizeof(filename), ".%s", path);

    const char *content_type = "text/plain";

    if (strstr(filename, ".html"))
        content_type = "text/html; charset=UTF-8";
    else if (strstr(filename, ".css"))
        content_type = "text/css; charset=UTF-8";
    else if (strstr(filename, ".js"))
        content_type = "application/javascript";

    FILE *file = fopen(filename, "rb");
    if (!file) {
        write(client_fd, HTTP_404_NOT_FOUND_RESPONSE,
              strlen(HTTP_404_NOT_FOUND_RESPONSE));
        close(client_fd);
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char header[256];
    snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n\r\n",
        content_type, file_size);

    write(client_fd, header, strlen(header));

    size_t n;
    while ((n = fread(buf->data, 1, buf->length, file)) > 0) {
        write(client_fd, buf->data, n);
    }

    fclose(file);
    close(client_fd);
}
