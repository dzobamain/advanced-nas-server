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
void handle_login(int client_fd, struct buffer *buf);
void serve_static_file(int client_fd, struct buffer *buf, const char *path);
void parse_post_data(const char *body, char *username, char *password);

#endif // CORE_H
