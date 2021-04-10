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
#include <regex>

using std::ifstream;
using std::filesystem::path;
using std::filesystem::file_size;
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::regex;
using std::regex_search;

// Number of threads
#define NUM_THREADS 4

// global shared memory for writing the file
char *shared_mem;

// global regex to use
regex reg;

// Arguments to pass to each thread
struct thread_data {
    size_t offset;
    size_t thsize;
    vector<string> v;
};

void *FindMatchingLines(void *threadargs) {
    // Retrieve thread arguments
    struct thread_data *td = (struct thread_data*)threadargs;
    size_t offset = td->offset;
    size_t thsize = td->thsize;

    // Initialize variables
    vector<string> v;
    string s = "";

    // Split on endlines
    for (size_t i = 0; i < thsize; ++i) {
        s += shared_mem[offset + i];
        // Line complete
        if (shared_mem[offset + i] == '\n') {
            // Line matched or first line in thread
            if (v.size() == 0 || regex_search(s, reg))
                v.push_back(s);
            s = "";
        }
    }

    // Add last incomplete line
    if (s.length() > 0)
        v.push_back(s);

    // store vector
    td->v = v;

    // exit thread
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 3) {
        perror("Error in number of arguments. Specify both file name and target word!");
        exit(1);
    }

    // Reads file name/size and target word from arguments
    string file_path = argv[1];
    string target_word = argv[2];
    size_t fsize = file_size(file_path);

    // Initialize regex
    reg = regex ("\\b" + target_word + "\\b", regex::icase);

    // Initialize shared memory for file and semaphore
    shared_mem = (char*)(mmap(NULL, file_size(file_path), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0));
    sem_t *semaphore = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, 0, 0 );
    sem_init(semaphore, 1, 0);

    // TODO shared memory?
    
    
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
        }

        // Release lock
        sem_post(semaphore);

        // Wait for child to complete
        wait(NULL);

        // Print results TODO
        /*for (string s: *results) {
            cout << "Printing Line..." << endl;
            cout << s << endl;
        }*/

    } else {
        // in child
        
        // Wait for parent to write to shared memory
        sem_wait(semaphore);

        // Thread and vector arrays for use in multithreading
        pthread_t threads[NUM_THREADS];
        struct thread_data thread_args[NUM_THREADS];

        // Create 4 threads
        for (int i = 0; i < NUM_THREADS; ++i) {
            // Create struct for thread arguments
            thread_args[i].offset = ((fsize - 1) / NUM_THREADS + 1) * i;
            thread_args[i].thsize = (i == NUM_THREADS - 1) ? fsize - thread_args[i].offset : (fsize - 1) / NUM_THREADS + 1;

            // Create thread
            pthread_create(&threads[i], NULL, &FindMatchingLines, (void *)&thread_args[i]);
        }

        /* Merge results */
        string intermediate = "";
        vector<string> results;
        for (size_t i = 0; i < NUM_THREADS; ++i) {

            // Get results of curr thread
            pthread_join(threads[i], NULL);
            vector<string> v = thread_args[i].v;

            // Loop over each string in the thread
            for (size_t j = 0; j < v.size(); ++j) {
                // i.e. this string is a standalone line - already matched
                if (j != 0 && intermediate.length() == 0 && v[j][v[j].length() - 1] == '\n') {
                    results.push_back(v[j]);
                // i.e. this string is a continuation or beginning w/o endl
                } else {
                    intermediate += v[j];
                    if (intermediate[intermediate.length() - 1] == '\n') {
                        // Test match since we couldn't do so in thread
                        if (regex_search(intermediate, reg))
                            results.push_back(intermediate);
                        intermediate = "";
                    }
                }
            }
        }
        // Test very last string in case file didn't end with endl
        if (regex_search(intermediate, reg))
                results.push_back(intermediate + '\n');

        // TODO remove
        for (string s : results) {
            cout << s;
        }
    }

    // Delete all remaining resources
    munmap(shared_mem, fsize);
    sem_destroy(semaphore);
    munmap(semaphore, sizeof(sem_t));

    return 0;
}
