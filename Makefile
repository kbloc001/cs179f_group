##
CC = gcc
CCP = g++
OPT = -D_FILE_OFFSET_BITS=64
PKG = `pkg-config fuse --cflags --libs`

myfs: bbfs.o log.o my_unix_commands.o
	$(CC) -lstdc++ bbfs.o log.o my_unix_commands.o $(PKG) -o myfs

bbfs.o: bbfs.c params.h my_unix_commands.h
	$(CC) $(OPT) -c bbfs.c

log.o: log.c log.h
	$(CC) $(OPT) -c log.c 

my_stubs.o: my_unix_commands.h my_unix_commands.cc
	$(CCP) -c my_stubs.cc

clean:
	rm myfs *.o
