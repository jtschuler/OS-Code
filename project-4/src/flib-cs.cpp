// Jadon Schuler
// Copyright 2021

#include <iostream>

#define BUF_SIZE 4096

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 2) {
        errno = EINVAL;
        perror("Argument num error. Specify only file name!");
        exit(1);
    }

    // Reads file name from arguments
    char * file = argv[1];

    // Create read file and write file
    FILE * f_read;
    FILE * f_write;

    // Open both files and check for any errors
    if ((f_read = fopen(file, "r+")) == NULL ||
            (f_write = fopen(file, "r+")) == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // Create buffer and related variables
    char buf[BUF_SIZE];
    size_t bytes_read, bytes_written;

    // Read from file until EOF
    while((bytes_read = fread(&buf, 1, BUF_SIZE, f_read)) > 0) {
        // Switch case of each character in the buffer
        for (size_t i = 0; i < bytes_read; ++i) {
            char c = buf[i];
            // Uppercase to lowercase
            if (c < 91 && c > 64)
                buf[i] = c + 32;
            // Lowercase to uppercase
            else if (c > 96 && c < 123)
                buf[i] = c - 32;
        }
        // Write the buffer back into the file and check for errors
        bytes_written = fwrite(&buf, 1, bytes_read, f_write);
        if (bytes_read != bytes_written) {
            perror("Error in file I/O");
            exit(1);
        }
    }

    // Close files
    fclose(f_read);
    fclose(f_write);

    return 0;
}