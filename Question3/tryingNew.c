#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netdb.h> // Include the netdb.h header file
#include <signal.h>

int start_tcp_server(int port)
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int new_socket;
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // char *command = argv[2];
    pid_t pid = fork();
    if (pid == 0)
    {
        char *args[] = {"ttt", "123456789", NULL};
        char *path = "../Question1/ttt";
        execvp(path, args);
        exit(EXIT_FAILURE);
    }
    else
    {
        char buffer[256];
        while (1)
        {
            ssize_t bytes_read = read(server_fd, buffer, 255);
            if (bytes_read <= 0)
            {
                break;
            }
            buffer[bytes_read] = '\0';
        }
    }

    waitpid(pid, NULL, 0);
    close(server_fd);
    return 0;
}

int start_tcp_client(const char *hostname, int port)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    struct hostent *he;
    char buffer[256];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    if ((he = gethostbyname(hostname)) == NULL)
    {
        printf("\nInvalid hostname\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    memcpy(&serv_addr.sin_addr, he->h_addr_list[0], he->h_length);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        printf("fork failed\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // child process reads from the server
        while (1)
        {
            ssize_t bytes_read = read(sock, buffer, 255);
            if (bytes_read <= 0)
            {
                break;
            }
            buffer[bytes_read] = '\0';
            printf("received from server %s", buffer);
        }
        close(sock);
        exit(0);
    }

    else
    { // write to server
        while (1)
        {
            printf("Enter posiotion to play. (-1 for exit)\n");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';

            if (strcmp(buffer, "-1") == 0)
            {
                break; // Exit loop if user enters -1
            }
            write(sock, buffer, strlen(buffer));
        }
        close(sock);
        kill(pid, SIGKILL);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s -e <command> [-i TCPS<port>] [-o TCPC<host,port>] [-b TCPS<port>]\n", argv[0]);
        return 1;
    }

    int in_fd = -1;
    int out_fd = -1;
    int both_fd = -1;
    char *command = NULL;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc)
        {
            command = argv[++i];
        }
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc)
        {
            printf("received i\n");
            if (strncmp(argv[i + 1], "TCPS", 4) == 0)
            {
                
                int port = atoi(argv[++i] + 4);
                in_fd = start_tcp_server(port);
                printf("starting server with port: %d", port);
            }
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            if (strncmp(argv[i + 1], "TCPC", 4) == 0)
            {
                char *host_port = argv[++i] + 4;
                char *host = strtok(host_port, ",");
                if (host != NULL)
                {
                    int port = atoi(strtok(NULL, ","));
                    out_fd = start_tcp_client(host, port);
                }
            }
        }
        else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc)
        {
            if (strncmp(argv[i + 1], "TCPS", 4) == 0)
            {
                int port = atoi(argv[++i] + 4);
                both_fd = start_tcp_server(port);
            }
        }
        else
        {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 1;
        }
    }

    if (command)
    {
        printf("Executing command: %s\n", command);
        // Here you would execute the command
    }
    else
    {
        fprintf(stderr, "No command specified with -e\n");
        return 1;
    }
    printf("input is %d, output is %d, both is %d\n", in_fd, out_fd, both_fd);
    int stdin_copy = dup(0);
    int stdout_copy = dup(1);

    if (in_fd != -1)
    {

        printf("im here");
        dup2(in_fd, 0);
        close(in_fd);
    }

    if (out_fd != -1)
    {
        printf("im here");

        dup2(out_fd, 1);
        close(out_fd);
    }

    if (both_fd != -1)
    {
        printf("im here");

        dup2(both_fd, 0);
        dup2(both_fd, 1);
        close(both_fd);
    }

    dup2(stdin_copy, 0);
    dup2(stdout_copy, 1);
    close(stdin_copy);
    close(stdout_copy);

    return 0;
}