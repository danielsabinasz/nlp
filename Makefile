CXX=g++
CXXFLAGS=-Wall -O2 -std=c++11
all: exercise01.4

exercise01.4: dictionary.o exercise01.4.cc
dictionary.o: dictionary.cc dictionary.hh

.PHONY: clean veryclean
clean:
	rm -f *.o

