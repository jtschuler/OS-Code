Jadon T Schuler
Copyright 2021

This program accepts two arguments, a text-file name and a target word. The
program searches through the file, and prints out each line that contains a
match for the target word, using fork, shared memory, and POSIX threading.

Link to demo video:
https://youtu.be/g5RerRHns6Y

To compile, run either of the following (as the first rule in the makefile is
the rule to create the executable):

make
make project3

To run the program, run the following:

./project3 <filename> <'target word'>
