all: a.out

a.out: my_stubs.cc
	reset
	g++ my_stubs.cc
	
clean:
	rm -rf a.out *.h.gch *.o
