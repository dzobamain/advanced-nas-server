#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define HTML_FILE "front/index.html"

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in addr;
    char buffer[4096];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(5050);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        return 1;
    }

    printf("Server running on http://localhost:8080\n");

    while (1)
    {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        read(client_fd, buffer, sizeof(buffer));

        // Open HTML file
        FILE *file = fopen(HTML_FILE, "r");
        if (!file)
        {
            const char *error =
                "HTTP/1.1 500 Internal Server Error\r\n"
                "Content-Type: text/plain\r\n\r\n"
                "Cannot open HTML file";
            write(client_fd, error, strlen(error));
            close(client_fd);
            continue;
        }

        const char *header =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Connection: close\r\n\r\n";
        write(client_fd, header, strlen(header));

        char file_buffer[1024];
        size_t bytes;
        while ((bytes = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0)
        {
            write(client_fd, file_buffer, bytes);
        }

        fclose(file);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
