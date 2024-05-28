#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

//  mync -e "ttt 123456789"
// This main should work only on the games from Q1.
int main(int argc, char* argv[]){

    if (argc != 4)
    {
        printf("usgae : 'mync -e 'ttt 123456789'\n");
        exit(1);
    }
    
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    char* path = "../Question1/ttt";
    char* exec_args[] = {argv[2], argv[3], NULL};

    if (pid == 0)
    {
        //child process
        execvp(path, exec_args);
        perror("execvp");
        exit(1);

    }

    if (pid > 0)   // parent process
    {
        waitpid(pid, NULL, 0);   // wait for the child process to finish
    }
    
    return 0;
}



void runTcpServer(char *host, int ip)
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = ip;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(host);
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                       (struct sockaddr *)&cli_addr,
                       &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    bzero(buffer, 256);
    n = read(newsockfd, buffer, 255);
    if (n < 0)
        error("ERROR reading from socket");
    printf("Here is the message: %s\n", buffer);
    n = write(newsockfd, "I got your message", 18);
    if (n < 0)
        error("ERROR writing to socket");
    close(newsockfd);
    close(sockfd);
}