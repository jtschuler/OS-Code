// Jadon T Schuler
// Copyright 2021

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::ifstream;
using std::filesystem::path;
using std::filesystem::file_size;
using std::string;
using std::vector;
using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 3) {
        perror("Error in number of arguments. Specify both file name and target word!");
        exit(1);
    }

    // Reads file name/size and target word from arguments
    string file_path = argv[1];
    string target_word = argv[2];
    size_t fsize = file_size(file_path);

    // Initialize shared memory for file and semaphore
    char *shared_mem = (char*)(mmap(NULL, file_size(file_path), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0));
    sem_t *semaphore = (sem_t*)mmap(0, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, 0, 0 );
    sem_init(semaphore, 1, 0);
    
    // Fork process and check for errors
    int pid = fork();
    if (pid < 0) {
        perror("Fork");
        exit(1);
    }

    if (pid > 0) {
        // in parent

        // Copy file data into shared memory
        ifstream file(file_path);
        for (size_t i = 0; i < fsize; ++i) {
            shared_mem[i] = file.get();
            cout << shared_mem[i];
        }
        cout << endl;

        // Release lock
        sem_post(semaphore);
        cout << "lock released." << endl;

        wait(NULL);

    } else {
        // in child
        
        // Wait for lock
        sem_wait(semaphore);
        cout << "Lock obtained by child" << endl;
    }

    // Delete all remaining resources
    munmap(shared_mem, fsize);
    sem_destroy(semaphore);

    return 0;
}
