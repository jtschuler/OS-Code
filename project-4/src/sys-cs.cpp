// Jadon Schuler
// Copyright 2021

#include <fcntl.h>
#include <unistd.h>

#include <iostream>

#define BUF_SIZE 4096

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 2) {
        errno = EINVAL;
        perror("Argument num error. Specify only file name!");
        exit(1);
    }

    // Reads file name/size and target word from arguments
    char * file = argv[1];

    int fd_read;
    int fd_write;

    if ((fd_read = open(file, O_RDONLY)) == -1 ||
            (fd_write = open(file, O_WRONLY)) == -1) {
        perror("Error opening file");
        exit(1);
    }

    char buf[BUF_SIZE];
    size_t bytes_read, bytes_written;

    while((bytes_read = read(fd_read, &buf, BUF_SIZE)) > 0) {
        for (size_t i = 0; i < bytes_read; ++i) {
            char c = buf[i];
            if (c < 91 && c > 64)
                buf[i] = c + 32;
            else if (c > 96 && c < 123)
                buf[i] = c - 32;
        }
        bytes_written = write(fd_write, &buf, bytes_read);
        if (bytes_read != bytes_written) {
            perror("Error in file I/O");
            exit(1);
        }
    }

    close(fd_read);
    close(fd_write);

    return 0;
}