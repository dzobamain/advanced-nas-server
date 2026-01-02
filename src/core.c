#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "config.h"
#include "core.h"
#include "logger.h"
#include "db.h"

int set_buffer_size(struct buffer* buf, size_t new_size)
{
    char *new_data = realloc(buf->data, new_size);
    if (!new_data) {
        return -1;
    }
    
    buf->data = new_data;
    buf->length = new_size;
    return 0;
}

int free_buffer(struct buffer* buf) 
{
    if (buf && buf->data) {
        free(buf->data);
        buf->data = NULL;
        buf->length = 0;
    }
    return 0;
}

int get_local_ip(char *buffer, size_t buflen) 
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return -1;

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        WSACleanup();
        return -1;
    }

    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        WSACleanup();
        return -1;
    }

    strncpy(buffer, inet_ntoa(*(struct in_addr *)host->h_addr_list[0]), buflen);
    buffer[buflen - 1] = '\0';
    WSACleanup();
    return 0;

#else
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return -1;

    struct sockaddr_in remote;
    remote.sin_family = AF_INET;
    remote.sin_port = htons(53); // DNS
    inet_pton(AF_INET, "8.8.8.8", &remote.sin_addr);

    if (connect(sock, (struct sockaddr *)&remote, sizeof(remote)) < 0) {
        close(sock);
        return -1;
    }

    struct sockaddr_in local;
    socklen_t len = sizeof(local);
    if (getsockname(sock, (struct sockaddr *)&local, &len) < 0) {
        close(sock);
        return -1;
    }

    inet_ntop(AF_INET, &local.sin_addr, buffer, buflen);
    close(sock);
    return 0;
#endif
}

int setup_address(struct sockaddr_in *addr, int port) 
{
    if (!addr)
        return -1;
    
    memset(addr, 0, sizeof(*addr));

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(port);

    return 0;
}

int create_server_socket(int port)
{
    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }

    // Setup address and bind
    // -
    struct sockaddr_in addr;
    if (setup_address(&addr, port) < 0) {
        close(server_fd);
        return -1;
    }
    //-
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    // Start listening
    if (listen(server_fd, BACKLOG) < 0) { 
        perror("listen");
        close(server_fd);
        return -1;
    }

    return server_fd;
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
