# Makefile super op

all: main.o everything exe

main.o: main.cpp
	g++ -std=c++11 -c main.cpp

everything:
	g++ -o game main.o -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lsfml-audio

exe:
	./game

clear:
	rm *.o game