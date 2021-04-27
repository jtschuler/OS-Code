Jadon T Schuler
Copyright 2021

Each program accepts one argument: a text-file name. The programs search
through the file, and switch the case of ever letter found.

Link to demo video:
https://youtu.be/zYvFPsAHoYk




sys-cs (Syscall Case-Switch):

This program uses syscalls (open, read, write, close) to perform the above
function.

To compile, run the following:

make sys-cs

To run the program, run the following:

./sys-cs <filename>




flib-cs (File Library Case-Switch):

This program uses syscalls (fopen, fread, fwrite, fclose) to perform the above
function.

To compile, run the following:

make flib-cs

To run the program, run the following:

./flib-cs <filename>




shmem-cs (Shared Memory Case-Switch):

This program uses shared memory (mmap) to perform the above function.

To compile, run the following:

make shmem-cs

To run the program, run the following:

./shmem-cs <filename>




I have provided a rule to compile all three programs at the same time:

make all

Or simply (as this is the first rule in the makefile):

make
