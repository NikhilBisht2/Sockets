#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#include <signal.h>
#pragma comment(lib, "ws2_32.lib")

// Global variables for cleanup
SOCKET sockfd = INVALID_SOCKET;
SOCKET newsockfd = INVALID_SOCKET;

void cleanup() {
    if (newsockfd != INVALID_SOCKET)
        closesocket(newsockfd);
    if (sockfd != INVALID_SOCKET)
        closesocket(sockfd);
    WSACleanup();
}

void interrupt_handler(int sig) {
    printf("\nShutting down server...\n");
    cleanup();
    exit(0);
}

void error(char *msg)
{
    perror(msg);
    cleanup();
    exit(1);
}

int main(int argc, char *argv[])
{
    // Set up Ctrl+C handler
    signal(SIGINT, interrupt_handler);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        error("WSAStartup failed");
    }
    
    int portno;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int clilen = sizeof(cli_addr);

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    
    listen(sockfd,5);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");
    
    memset(buffer, 0, 256);
    n = recv(newsockfd, buffer, 255, 0);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    
    n = send(newsockfd, "I got your message", 18, 0);
    if (n < 0) error("ERROR writing to socket");
    
    closesocket(newsockfd);
    WSACleanup();
    return 0; 
}
