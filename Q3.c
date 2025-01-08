#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT "69"
#define BUFFER_SIZE 512

// Create a UDP socket
int createSocket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully.\n");
    return sockfd;
}

// Construct a TFTP RRQ (Read Request)
int constructRRQ(const char *filename, char *buffer) {
    int offset = 0;

    // Opcode (01 for RRQ)
    buffer[offset++] = 0;
    buffer[offset++] = 1;

    // Filename
    strcpy(buffer + offset, filename);
    offset += strlen(filename) + 1;

    // Transfer mode ("octet")
    strcpy(buffer + offset, "octet");
    offset += strlen("octet") + 1;

    return offset; // Total request size
}

// Send the RRQ to the server
void sendRRQ(int sockfd, const char *buffer, int bufferSize, const struct sockaddr_in *serverAddr) {
    ssize_t sentBytes = sendto(sockfd, buffer, bufferSize, 0, (struct sockaddr *)serverAddr, sizeof(*serverAddr));
    if (sentBytes == -1) {
        perror("Error sending RRQ");
        exit(EXIT_FAILURE);
    }
    printf("RRQ sent successfully (%ld bytes).\n", sentBytes);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Create a UDP socket
    int sockfd = createSocket();

    // Construct the RRQ
    char buffer[BUFFER_SIZE];
    int reqSize = constructRRQ(argv[2], buffer);
    printf("RRQ constructed (size: %d bytes).\n", reqSize);

    // Configure server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(69);

    if (inet_pton(AF_INET, argv[1], &serverAddr.sin_addr) <= 0) {
        perror("Invalid server address");
        exit(EXIT_FAILURE);
    }

    // Send the RRQ
    sendRRQ(sockfd, buffer, reqSize, &serverAddr);

    close(sockfd);
    return EXIT_SUCCESS;
}