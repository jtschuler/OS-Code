CC = g++			# Compiler to use

CFLAGS = -std=c++17	# Use the C17 Standard
CFLAGS += -Wall		# compile with all warnings
CFLAGS += -I .		# search for included files in the current directory
CFLAGS += -g		# Add gdb support

LINK = $(CC) $(CFLAGS) -o			# build executable
COMPILE = $(CC) $(CFLAGS) -c -o		# compile to .o files


all:
	make sys-cs
	make flib-cs
	make shmem-cs

sys-cs: bin/sys-cs.o
	$(LINK) $@ $^

bin/sys-cs.o: src/sys-cs.cpp bin
	$(COMPILE) $@ $<

flib-cs: bin/flib-cs.o
	$(LINK) $@ $^

bin/flib-cs.o: src/flib-cs.cpp bin
	$(COMPILE) $@ $<

shmem-cs: bin/shmem-cs.o
	$(LINK) $@ $^

bin/shmem-cs.o: src/shmem-cs.cpp bin
	$(COMPILE) $@ $<

lint: src/*.cpp
	cpplint --root=. $^

clean:
	rm -f bin/* sys-cs flib-cs shmem-cs

bin:
	mkdir bin

archive:
	make clean
	tar -cvzf ../project-4.tar.gz ../project-4/

