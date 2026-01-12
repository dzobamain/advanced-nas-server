#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logger.h"
#include "http.h"
#include "config.h"
#include "db.h"

struct session {
    char sid[32];
};

static struct session sessions[16];
static int session_count = 0;

static void generate_sid(char *out) 
{
    snprintf(out, 32, "%ld", random());
}

static int has_valid_session(const char *req) 
{
    const char *cookie = strstr(req, "Cookie:");
    if (!cookie) 
        return 0;

    for (int i = 0; i < session_count; i++) {
        if (strstr(cookie, sessions[i].sid))
            return 1;
    }
    return 0;
}

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

    /* GET / → login */
    if (!strcmp(method, "GET") && !strcmp(path, "/")) {
        serve_file(client_fd, buf, LOGIN_HTML_FILE);
        return;
    }

    /* POST /login */
    if (!strcmp(method, "POST") && !strcmp(path, "/login")) {
        handle_login(client_fd, buf);
        return;
    }

    /* GET /storage */
    if (!strcmp(method, "GET") && !strcmp(path, "/storage")) {
        handle_storage(client_fd, buf);
        return;
    }

    /* static (css/js only) */
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
    memset(&usr, 0, sizeof(usr));

    sscanf(body, "username=%63[^&]&password=%63s",
           usr.username, usr.password);

    if (!is_user_in_db(DATABASE_FILE, &usr)) {
        const char *resp =
            "HTTP/1.1 302 Found\r\n"
            "Location: /\r\n"
            "Connection: close\r\n\r\n";
        write(client_fd, resp, strlen(resp));
        close(client_fd);
        return;
    }

    char sid[32];
    generate_sid(sid);

    strcpy(sessions[session_count].sid, sid);
    session_count++;

    char resp[256];
    snprintf(resp, sizeof(resp),
        "HTTP/1.1 302 Found\r\n"
        "Set-Cookie: sid=%s; Path=/; HttpOnly\r\n"
        "Location: /storage\r\n"
        "Connection: close\r\n\r\n",
        sid);

    write(client_fd, resp, strlen(resp));
    close(client_fd);
}


void handle_storage(int client_fd, struct buffer *buf)
{
    if (!has_valid_session(buf->data)) {
        const char *resp =
            "HTTP/1.1 302 Found\r\n"
            "Location: /\r\n"
            "Connection: close\r\n\r\n";
        write(client_fd, resp, strlen(resp));
        close(client_fd);
        return;
    }

    serve_file(client_fd, buf, STORAGE_HTML_FILE);
}

void serve_file(int client_fd, struct buffer *buf, const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file) {
        write(client_fd, HTTP_404_NOT_FOUND_RESPONSE,
              strlen(HTTP_404_NOT_FOUND_RESPONSE));
        close(client_fd);
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char header[256];
    snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n\r\n",
        size);

    write(client_fd, header, strlen(header));

    size_t n;
    while ((n = fread(buf->data, 1, buf->length, file)) > 0)
        write(client_fd, buf->data, n);

    fclose(file);
    close(client_fd);
}

void serve_static_file(int client_fd, struct buffer *buf, const char *path)
{
    if (strstr(path, "storage.html")) {
        write(client_fd, HTTP_404_NOT_FOUND_RESPONSE,
              strlen(HTTP_404_NOT_FOUND_RESPONSE));
        close(client_fd);
        return;
    }

    char filename[256];
    snprintf(filename, sizeof(filename), ".%s", path);

    const char *type = "text/plain";
    if (strstr(filename, ".css")) type = "text/css";
    else if (strstr(filename, ".js")) type = "application/javascript";

    FILE *file = fopen(filename, "rb");
    if (!file) {
        write(client_fd, HTTP_404_NOT_FOUND_RESPONSE,
              strlen(HTTP_404_NOT_FOUND_RESPONSE));
        close(client_fd);
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char header[256];
    snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n\r\n",
        type, size);

    write(client_fd, header, strlen(header));

    size_t n;
    while ((n = fread(buf->data, 1, buf->length, file)) > 0)
        write(client_fd, buf->data, n);

    fclose(file);
    close(client_fd);
}
