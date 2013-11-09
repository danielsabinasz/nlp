CXX=g++
CXXFLAGS=-Wall -O2 -std=c++11
all: exercise01.4 exercise03

exercise01.4: dictionary.o exercise01.4.cc
exercise03: dictionary.o exercise03.cc
dictionary.o: dictionary.cc dictionary.hh

.PHONY: clean veryclean
clean:
	rm -f *.o

