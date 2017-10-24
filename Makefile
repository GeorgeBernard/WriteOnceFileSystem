CFLAGS=  -std=c++11

##============================== Infrastructure ==============================##

all: generate.out tree.out master.o

rebuild: clean all

clean:
	rm -f *.o *.out *.wfs

##================================ Executables ===============================##

generate.out: generate.cpp
	g++ $(CFLAGS) generate.cpp -o generate.out

tree.out: tree.cpp
	g++ $(CFLAGS) tree.cpp -o tree.out

master.o: Master.cpp
	g++ $(CFLAGS) Master.cpp -o master.out
