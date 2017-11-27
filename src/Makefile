CFLAGS= -std=c++11 -g

all: master.out mounter

master.out: master.cpp
	g++ $(CFLAGS) master.cpp -o master.out -lcrypto

mounter: mounter.c
	g++  $(CFLAGS) -Wall -g mounter.c `pkg-config fuse3 --cflags --libs` -o mounter.out -lcrypto 
