all: a.out

a.out: my_unix_commands.o  my_bbfs.o
	g++ my_bbfs.o my_unix_commands.o -o a.out

my_bbfs.o: my_bbfs.c my_unix_commands.h
	gcc -D_FILE_OFFSET_BITS=64 -c my_bbfs.c -o my_bbfs.o
my_unix_commands.o: my_unix_commands.cc my_unix_commands.h
	g++ -c my_unix_commands.cc -o my_unix_commands.o

clean:
	rm -rf *.o *.out 