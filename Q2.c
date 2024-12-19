#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PORT "69"
#define MAX_CHAR_SIZE 128

// Function to resolve the hostname and display IP/port information
void resolveHostname(const char *hostname) {
    struct addrinfo hints, *result;
    char ipAddress[MAX_CHAR_SIZE];
    char port[MAX_CHAR_SIZE];

    // Initialize the hints structure
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP
    hints.ai_protocol = IPPROTO_UDP;

    // Resolve the hostname
    int status = getaddrinfo(hostname, PORT, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "Error resolving hostname %s: %s\n", hostname, gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    // Convert the address to a readable format
    status = getnameinfo(result->ai_addr, result->ai_addrlen, ipAddress, sizeof(ipAddress), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);
    if (status != 0) {
        fprintf(stderr, "Error converting address: %s\n", gai_strerror(status));
        freeaddrinfo(result);
        exit(EXIT_FAILURE);
    }

    // Display the IP and port
    printf("Communication with: %s:%s\n", ipAddress, port);

    // Free allocated memory
    freeaddrinfo(result);
}

int main(int argc, char **argv) {
    // Debugging: Display all arguments
    printf("Number of arguments (argc): %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }

    // Validate the number of arguments
    if (argc != 2) { // Expecting exactly 1 argument after the program name
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Resolve and display information
    resolveHostname(argv[1]);
    return EXIT_SUCCESS;
}

