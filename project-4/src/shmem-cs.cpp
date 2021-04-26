// Jadon Schuler
// Copyright 2021

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 2) {
        errno = EINVAL;
        perror("Argument num error. Specify only file name!");
        exit(1);
    }

    // Get file name from arguments and open file descriptor
    char * file = argv[1];
    int fd = open(file, O_RDWR);

    // Get file size using fstat
    struct stat finfo;
    fstat(fd, &finfo);
    size_t length = finfo.st_size;

    // Open shared memory and close file descriptor
    char *shmem = (char *) mmap(NULL, length, PROT_READ | PROT_WRITE,
                                                    MAP_SHARED, fd, 0);
    close(fd);

    // Switch case of every character
    for (size_t i = 0; i < length; ++i) {
        char c = shmem[i];
        if (c < 91 && c > 64)
            shmem[i] = c + 32;
        else if (c > 96 && c < 123)
            shmem[i] = c - 32;
    }

    // Make absolutely SURE the changes were written
    // Not strictly necessary but I added this just in case
    msync(shmem, length, MS_SYNC);

    // Unmap memory
    munmap(shmem, length);

    return 0;
}