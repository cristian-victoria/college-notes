CC=gcc
CXX=g++
CXXFLAGS=-std=c++17 -Wall -pedantic

proj1.x: proj1.o
	$(CXX) $(CXXFLAGS) -o proj1.x proj1.o

proj1.o: proj1.cpp
	$(CXX) $(CXXFLAGS) -c -o proj1.o proj1.cpp

.SUFFIXES: .x

.PHONY : clean

clean:
	rm -f *.o *.x core.*

%.x : %.c
	$(CC) -o $@ $<
%.x : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
