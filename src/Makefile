CFLAGS= -std=c++11 -g

##============================== Infrastructure ==============================##

all: generate.out tree.out master.out hello

rebuild: clean all

clean:
	rm -f *.o *.out *.wfs hello

##================================ Executables ===============================##

generate.out: generate.cpp
	g++ $(CFLAGS) generate.cpp -o generate.out

tree.out: tree.cpp
	g++ $(CFLAGS) tree.cpp -o tree.out

master.out: Master.cpp
	g++ $(CFLAGS) Master.cpp -o master.out

hello: hello.c
	gcc -Wall -g hello.c `pkg-config fuse3 --cflags --libs` -o hello