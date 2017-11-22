CFLAGS= -std=c++11 -g

master.out: Master.cpp
	g++ $(CFLAGS) Master.cpp -o master.out -lcrypto

hello: hello.c
	gcc -Wall -g hello.c `pkg-config fuse3 --cflags --libs` -o hello
