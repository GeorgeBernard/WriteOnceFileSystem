CFLAGS= -Wall -Wextra -std=c++11

all: master.o old.o

master.o: Master.cpp
	g++ $(CFLAGS) Master.cpp -o master.out

old.o: old.cpp
	g++ $(CFLAGS) old.cpp -o old.out

rebuild: clean all

clean:
	rm -f *.o *.out
