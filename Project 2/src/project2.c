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
        printf("In parent\n");

        // Open file
        int file = open(file_path, O_RDONLY);
        if (file == -1) {
            perror("Error opening provided file.");
            kill(pid, SIGKILL);
            exit(1);
        }

        // Create buffer to use with reading file
        char buffer[BUF_SIZE];

        // Read data from file into socket
        ssize_t bytes_read, bytes_written;
        while ((bytes_read = read(file, &buffer, BUF_SIZE)) > 0) {
            bytes_written = write(sv[0], &buffer, bytes_read);
            if (bytes_written != bytes_read) {
                perror("Error while writing data to socket");
                kill(pid, SIGKILL);
                exit(1);
            }
        }

        // Send end of file
        write(sv[0], "\0", 1);
        printf("Parent finished writing to socket\n");
        close(file);

        // Wait for child, and check exit status
        printf("Parent waiting on child...\n");
        int status;
        wait(&status);
        if (status != 0) {
            perror("Error in child! Parent exiting...\n");
            exit(1);
        }
        printf("\nChild process reaped.\n");

        // Print lines written to socket
        printf("Parent continuing...\n");
        printf("\nMatching Lines:\n");
        while ((bytes_read = read(sv[0], &buffer, BUF_SIZE)) > 0) {
            write(1, &buffer, bytes_read);
            if (buffer[bytes_read - 1] == '\0') {
                break;
            }
        }

    } else {
        // in child
        printf("\nIn child\n");
        printf("\nReading data from socket stream...\n");

        // Read from socket
        char buffer[BUF_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(sv[1], &buffer, BUF_SIZE)) > 0) {
            printf("\n%ld bytes read:\n", bytes_read);
            write(1, &buffer, bytes_read);
            if (buffer[bytes_read - 1] == '\0') {
                break;
            }
        }
        printf("\n");

        // regex building
        // len: 8 + 4*len(target) + 1 (for null char)
        printf("\nTarget: '%s'\n", target_word);
        char pattern[BUF_SIZE];
        int pos;
        int size = BUF_SIZE;
        pos = snprintf(pattern, size, ".*\\b");
        size -= pos;
        char add[5];
        for (int i = 0; i < strlen(target_word); ++i) {
            char curr = target_word[i];
            add[0] = '[';
            add[1] = toupper(curr);
            add[2] = tolower(curr);
            add[3] = ']';
            add[4] = '\0';
            pos += snprintf(pattern + pos, size, "%s", add);
            size -= pos;
        }
        pos += snprintf(pattern + pos, size, "\\b.*");
        if (pos > BUF_SIZE) {
            perror("Regex buffer size exceeded\n");
            exit(1);
        }


        printf("Regex: %s\n", pattern);
        regex_t regex;
        if (regcomp(&regex, pattern, 0) == 0) {
            printf("Regex compilation successful\n\n");
        } else {
            perror("Regex compilaton failure\n");
            exit(1);
        }

        // Check lines and write to socket
        ssize_t bytes_written, line_length;
        char * line;
        char * state;
        line = strtok_r(buffer, "\n", &state);
        do {
            // Match
            if (regexec(&regex, line, 0, NULL, 0) == 0) {
                printf("Match on '%s'\n", line);
                // write data to socket
                line_length = strlen(line);
                bytes_written = write(sv[1], line, line_length);
                if (bytes_written != line_length) {
                    perror("Error while writing data to socket\n");
                    regfree(&regex);
                    exit(1);
                }
                write(sv[1], "\n", 1);
            } else {
                printf("No match on '%s'\n", line);
            }
        } while ((line = strtok_r(NULL, "\n", &state)));

        // Write end of file
        write(sv[1], "\0", 1);

        // delete regex
        regfree(&regex);
    }

    if (pid == 0)
        printf("Child process ended.\n");
    return 0;
}
