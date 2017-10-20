CFLAGS= -Wall -Wextra -std=c++17

all: generate.o tree.o

generate.o: generate.cpp
	g++ $(CFLAGS) generate.cpp -o generate.out

tree.o: tree.cpp
	g++ $(CFLAGS) tree.cpp -o tree.out


rebuild: clean all

clean:
	rm -f *.o *.out