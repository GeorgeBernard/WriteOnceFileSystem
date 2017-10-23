CFLAGS= -Wall -Wextra -std=c++17

all: master.o

master.o: Master.cpp
	g++ $(CFLAGS) Master.cpp -o master.out

rebuild: clean all

clean:
	rm -f *.o *.out