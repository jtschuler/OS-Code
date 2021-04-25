CC = g++			# Compiler to use

CFLAGS = -std=c++17	# Use the C17 Standard
CFLAGS += -Wall		# compile with all warnings
CFLAGS += -I .		# search for included files in the current directory
CFLAGS += -g		# Add gdb support

LINK = $(CC) $(CFLAGS) -o			# build executable
COMPILE = $(CC) $(CFLAGS) -c -o		# compile to .o files


all:
	make sys-lc
	make flib-lc
	make shmem-lc

sys-lc: bin/sys-lc.o
	$(LINK) $@ $^

bin/sys-lc.o: src/sys-lc.cpp bin
	$(COMPILE) $@ $<

flib-lc: bin/flib-lc.o
	$(LINK) $@ $^

bin/flib-lc.o: src/flib-lc.cpp bin
	$(COMPILE) $@ $<

shmem-lc: bin/shmem-lc.o
	$(LINK) $@ $^

bin/shmem-lc.o: src/shmem-lc.cpp bin
	$(COMPILE) $@ $<

lint: src/*.cpp
	cpplint --root=. $^

clean:
	rm -f bin/* sys-lc flib-lc shmem-lc

bin:
	mkdir bin

archive:
	make clean
	tar -cvzf ../project-4.tar.gz ../project-4/

