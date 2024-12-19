#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

// Macros for error messages
#define NOT_ENOUGH_ARGS "Not enough arguments or too many arguments | Required: 3 arguments\n"
#define NO_FILE "File does not exist. Please repeat the process.\n"
#define NOT_REGULAR "Warning! File is not a regular file.\n"

// File statistics structure
struct stat sbFileInput;

// Function to validate input arguments and file
void checkFormat(int argc, char **argv) {
    // Check if the number of arguments is correct
    if (argc != 4) {
        fprintf(stderr, "%s", NOT_ENOUGH_ARGS);
        exit(EXIT_FAILURE);
    }

    // These checks apply only to the "put" request
    if (strcmp(argv[1], "put") == 0) {
        // Check if the file exists
        if (stat(argv[3], &sbFileInput) == -1) {
            perror(NO_FILE); // Display a detailed system error
            exit(EXIT_FAILURE);
        }

        // Check if the file is a regular file
        if (!S_ISREG(sbFileInput.st_mode)) {
            fprintf(stderr, "%s", NOT_REGULAR);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv) {
    // Validate input arguments
    checkFormat(argc, argv);

    // If no errors, print success message
    printf("Arguments and file format validated successfully.\n");

    return 0;
}