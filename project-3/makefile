CC = g++			# Compiler to use

CFLAGS = -std=c++17	# Use the C17 Standard
CFLAGS += -Wall		# compile with all warnings
CFLAGS += -I .		# search for included files in the current directory
CFLAGS += -g		# Add gdb support
CFLAGS += -pthread	# Add semaphore support (as well as other threading features)

LINK = $(CC) $(CFLAGS) -o			# build executable
COMPILE = $(CC) $(CFLAGS) -c -o		# compile to .o files


project3: bin/project3.o
	$(LINK) $@ $^

bin/project3.o: src/project3.cpp bin
	$(COMPILE) $@ $<

lint: src/project3.cpp
	cpplint --root=. $^

clean:
	rm -f bin/* project3

bin:
	mkdir bin

archive:
	make clean
	tar -cvzf ../project-3.tar.gz ../project-3

