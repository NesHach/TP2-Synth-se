#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <errno.h>

#define PORT "69"
#define BUFFER_SIZE 516 // 512 bytes for data + 4 bytes for header

// Creates a UDP socket and returns the file descriptor
int createSocket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully.\n");
    return sockfd;
}

// Constructs a Write Request (WRQ) packet for TFTP
int constructWRQ(const char *filename, char *buffer) {
    int offset = 0;

    buffer[offset++] = 0;  // Opcode: 2 for WRQ
    buffer[offset++] = 2;

    strcpy(buffer + offset, filename); // Add the filename
    offset += strlen(filename) + 1;

    strcpy(buffer + offset, "octet");  // Add the mode ("octet" for binary transfer)
    offset += strlen("octet") + 1;

    return offset; // Total WRQ packet size
}

// Sends a file to the TFTP server
void sendFile(int sockfd, struct sockaddr_in *serverAddr, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    char ackBuffer[BUFFER_SIZE];
    socklen_t addrLen = sizeof(*serverAddr);
    ssize_t receivedBytes;
    int blockNumber = 0;

    // Set a timeout for receiving data
    struct timeval timeout = {5, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while (1) {
        // Read data from the file (up to 512 bytes)
        int dataSize = fread(buffer + 4, 1, 512, file);
        if (dataSize == 0 && feof(file)) {
            break; // End of file
        }

        // Prepare a DATA packet
        buffer[0] = 0;
        buffer[1] = 3; // Opcode: 3 for DATA
        buffer[2] = (blockNumber >> 8) & 0xFF; // Block number (high byte)
        buffer[3] = blockNumber & 0xFF;        // Block number (low byte)

        // Send the DATA packet
        ssize_t sentBytes = sendto(sockfd, buffer, dataSize + 4, 0, (struct sockaddr *)serverAddr, addrLen);
        if (sentBytes == -1) {
            perror("Error sending data");
            fclose(file);
            exit(EXIT_FAILURE);
        }
        printf("Sent block %d (%ld bytes).\n", blockNumber, sentBytes);

        // Receive ACK with retry mechanism
        int retries = 3; // Number of retry attempts
        while (retries > 0) {
            receivedBytes = recvfrom(sockfd, ackBuffer, BUFFER_SIZE, 0, (struct sockaddr *)serverAddr, &addrLen);
            if (receivedBytes == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // Timeout: Retry receiving ACK
                    fprintf(stderr, "ACK not received, retrying... (%d retries left)\n", retries - 1);
                    retries--;
                    continue;
                } else {
                    perror("Error receiving ACK");
                    fclose(file);
                    exit(EXIT_FAILURE);
                }
            }
            break; // ACK successfully received
        }
        if (retries == 0) {
            fprintf(stderr, "Failed to receive ACK after multiple attempts.\n");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        // Check ACK packet validity
        int opcode = (ackBuffer[0] << 8) | ackBuffer[1];
        if (opcode == 5) { // Error packet
            fprintf(stderr, "Server error: %s\n", ackBuffer + 4);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        int ackBlock = (ackBuffer[2] << 8) | ackBuffer[3];
        if (opcode != 4 || ackBlock != blockNumber) { // Invalid ACK block
            fprintf(stderr, "Error: Incorrect ACK received (block %d).\n", blockNumber);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        printf("Received ACK for block %d.\n", blockNumber);
        blockNumber++; // Proceed to the next block
    }

    fclose(file);
    printf("File transfer complete.\n");
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *serverIp = argv[1];
    const char *filename = argv[2];

    // Create a UDP socket
    int sockfd = createSocket();

    // Construct the WRQ packet
    char buffer[BUFFER_SIZE];
    int reqSize = constructWRQ(filename, buffer);
    printf("WRQ constructed (size: %d bytes).\n", reqSize);

    // Initialize the server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(69);

    if (inet_pton(AF_INET, serverIp, &serverAddr.sin_addr) <= 0) {
        perror("Invalid server address");
        exit(EXIT_FAILURE);
    }

    // Send the WRQ packet to the server
    sendto(sockfd, buffer, reqSize, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    printf("WRQ sent successfully.\n");

    // Start the file transfer process
    sendFile(sockfd, &serverAddr, filename);

    // Close the socket
    close(sockfd);
    return EXIT_SUCCESS;
}