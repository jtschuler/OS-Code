CC = gcc			# Compiler to use

CFLAGS = -std=c17	# Use the C17 Standard
CFLAGS += -Wall		# compile with all warnings
CFLAGS += -I .		# search for included files in the current directory
CFLAGS += -g		# Add gdb support
CFLAGS += -pthread	# Add semaphore support (as well as other threading features)

LINK = $(CC) $(CFLAGS) -o			# build executable
COMPILE = $(CC) $(CFLAGS) -c -o		# compile to .o files


project2: bin/project2.o
	$(LINK) $@ $^

bin/project2.o: src/project2.c
	$(COMPILE) $@ $<

lint: src/project2.c
	cpplint --root=. $^

clean:
	rm -f bin/* hello