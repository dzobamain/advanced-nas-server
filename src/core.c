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
