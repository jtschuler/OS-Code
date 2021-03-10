// Copyright 2021 Jadon T Schuler

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <regex.h>
#include <string.h>
#include <ctype.h>

#define BUF_SIZE 4096

int main(int argc, char* argv[]) {
    if (argc < 3) {
        perror("Not enough arguments. Specify both file name and target word!");
        exit(1);
    }

    // Opens file and reads target word from arguments
    char* file_path = argv[1];
    char* target_word = argv[2];

    // Set up socket pair
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        perror("Socket failed");
        exit(1);
    }

    // Fork process and check for errors
    int pid = fork();
    if (pid < 0) {
        perror("Fork");
        exit(1);
    }

    if (pid > 0) {
        // in parent
        // Open file
        int file = open(file_path, O_RDONLY);
        if (file == -1) {
            perror("Error opening provided file.\n");
            kill(pid, SIGKILL);
            exit(1);
        }

        // Create/initialize buffers to use with reading file and writing to stdout
        char buffer[BUF_SIZE];
        for (int i = 0; i < BUF_SIZE; ++i) {
            buffer[i] = '\0';
        }
        char read_buffer[BUF_SIZE];
        for (int i = 0; i < BUF_SIZE; ++i) {
            read_buffer[i] = '\0';
        }

        printf("\nMatching Lines:\n");

        // Read data from file into socket
        ssize_t bytes_read_file, bytes_read_socket, bytes_written;
        while ((bytes_read_file = read(file, &buffer, BUF_SIZE)) > 0) {
            bytes_written = write(sv[0], &buffer, bytes_read_file);
            if (bytes_written != bytes_read_file) {
                perror("Error while writing data to socket\n");
                kill(pid, SIGKILL);
                exit(1);
            }
            // Write matches from the current block
            while ((bytes_read_socket = read(sv[0], &read_buffer, BUF_SIZE)) > 0) {
                write(1, &read_buffer, bytes_read_socket);
                if (read_buffer[bytes_read_socket - 1] == '\0')
                    break;
            }
        }

        // Send end of file (and extra newline just in case)
        write(sv[0], "\n\0", 2);
        close(file);

        // Print final lines written from socket to stdout
        while ((bytes_read_socket = read(sv[0], &read_buffer, BUF_SIZE)) > 0) {
            write(1, &read_buffer, bytes_read_socket);
            if (read_buffer[bytes_read_socket - 1] == '\0') {
                break;
            }
        }

    } else {
        // in child

        // regex building
        // length: 4 + 4*len(target) + 1 (for null char)
        int size = 5 + 4*strlen(target_word);
        char pattern[size];
        strcpy(pattern, "\\b");
        char add[5];
        for (int i = 0; i < strlen(target_word); ++i) {
            char curr = target_word[i];
            add[0] = '[';
            add[1] = toupper(curr);
            add[2] = tolower(curr);
            add[3] = ']';
            add[4] = '\0';
            strcat(pattern, add);
        }
        strcat(pattern, "\\b\0");
        regex_t regex;
        if (regcomp(&regex, pattern, 0) != 0) {
            perror("Regex compilaton failure\n");
            exit(1);
        }

        // Create and initialize buffers/variables for socket use
        char buffer[BUF_SIZE];
        for (int i = 0; i < BUF_SIZE; ++i) {
            buffer[i] = '\0';
        }
        char line[BUF_SIZE];
        for (int i = 0; i < BUF_SIZE; ++i) {
            line[i] = '\0';
        }
        ssize_t bytes_read, bytes_written, line_length;
        int start = 0;
        int line_finished = 1;

        // Loop to read incoming socket data
        while ((bytes_read = read(sv[1], &buffer, BUF_SIZE)) > 0) {
            // Break on lines and match pattern
            start = 0;
            for (int i = 0; i < bytes_read; ++i) {
                if (buffer[i] == '\n') {
                    if (line_finished == 1)
                        strncpy(line, buffer+start, i - start);
                    else
                        strncat(line, buffer+start, i - start);
                    line_length = strlen(line);
                    line_finished = 1;
                    if (regexec(&regex, line, 0, NULL, 0) == 0) {
                        // write data to socket
                        bytes_written = write(sv[1], line, line_length);
                        write(sv[1], "\n", 1);
                        if (bytes_written != line_length) {
                            perror("Error while writing data to socket\n");
                            regfree(&regex);
                            exit(1);
                        }
                    }
                    for (int j = 0; j < i - start; ++j) {
                        line[j] = '\0';
                    }
                    start = i + 1;
                }
            }

            // Make sure this line actually completed
            if (start < bytes_read) {
                strncpy(line, buffer+start, bytes_read - start);
                line_finished = 0;
            }

            // Write to signal finished with block
            write(sv[1], "\0", 1);

            // End of file recieved
            if (buffer[bytes_read - 1] == '\0') {
                break;
            }
        }

        // delete regex
        regfree(&regex);
    }

    if (pid == 0) {
        // Close file descriptors
        close(sv[1]);
    } else {
        // Close file descriptors
        close(sv[0]);

        // Wait and check child exit status
        int status;
        wait(&status);
        if (status != 0) {
            perror("Error in child!\n");
            exit(1);
        }

        // Program finished
        printf("\nDone.\n");
    }
    return 0;
}
