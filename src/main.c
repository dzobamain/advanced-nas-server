#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "core.h"
#include "config.h"

int main() 
{
    int server_fd = create_server_socket(PORT);
    if (server_fd < 0)
        return server_fd;

    char local_ip[INET_ADDRSTRLEN];
    get_local_ip(local_ip, sizeof(local_ip));
    printf("Server running on http://%s:%d\n", local_ip, PORT);

    struct buffer buf = {0};
    set_buffer_size(&buf, BUFFER_SIZE);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        handle_client(client_fd, &buf);
    }

    free_buffer(&buf);
    close(server_fd);
    return 0;
}
