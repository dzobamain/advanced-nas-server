#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

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
int start_listening(int server_fd, struct sockaddr_in *addr);

#endif // SERVER_UTILS_H
