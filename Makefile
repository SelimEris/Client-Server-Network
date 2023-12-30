# Used the linkedlist makefile from homework 5 as a reference to build this makefile
all: server

server: libpack109.a server.o
	g++ build/objects/release/server.o -o server -lpack109 -Lbuild/lib/release -std=c++11
	mkdir -p build/bin/release
	mv server build/bin/release/server

libpack109.a:
	g++ src/lib.cpp -c -Ilib -std=c++11
	ar rs libpack109.a lib.o 
	mkdir -p build/lib/release
	mkdir -p build/objects/release
	mv *.o build/objects/release
	mv libpack109.a build/lib/release

server.o:
	g++ src/server.cpp -c -lpack109 -Lbuild/lib/release -Ilib -std=c++11
	mkdir -p build/objects/release
	mv server.o build/objects/release

clean:
	rm -f *.a
	rm -f *.o
	rm -rf build