CFLAGS= -Wall -Wextra 

all: master.o

master.o: master.cpp
	g++ $(CFLAGS) master.cpp -o master.out

rebuild: clean all

clean:
	rm -f *.o *.out