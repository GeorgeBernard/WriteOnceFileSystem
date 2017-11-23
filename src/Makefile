CFLAGS= -std=c++11 -g

all: master.out hello

master.out: Master.cpp
	g++ $(CFLAGS) Master.cpp -o master.out -lcrypto

hello: hello.c
	gcc -Wall -g hello.c `pkg-config fuse3 --cflags --libs` -o hello -lcrypto
