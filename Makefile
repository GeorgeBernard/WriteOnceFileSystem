CFLAGS= -Wall -Wextra -std=c++17

##============================== Infrastructure ==============================##

all: generate.out tree.out

rebuild: clean all

clean:
	rm -f *.o *.out *.wfs

##================================ Executables ===============================##

generate.out: generate.cpp
	g++ $(CFLAGS) generate.cpp -o generate.out

tree.out: tree.cpp
	g++ $(CFLAGS) tree.cpp -o tree.out


