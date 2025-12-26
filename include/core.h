#ifndef CORE_H
#define CORE_H

#include <netinet/in.h>

struct buffer
{
    size_t length;
    char* data;
};

int set_buffer_size(struct buffer* buf, size_t new_size);
int free_buffer(struct buffer* buf);

int get_local_ip(char *buffer, size_t buflen);
int create_server_socket(int port);
int setup_address(struct sockaddr_in *addr, int port);
void handle_client(int client_fd, struct buffer *buf);

#endif // CORE_H
