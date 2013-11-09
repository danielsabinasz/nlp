CXX=g++
CXXFLAGS=-Wall -O2 -std=c++11
all: trainer

trainer: dictionary.o trainer.cc
dictionary.o: dictionary.cc dictionary.hh

.PHONY: clean veryclean
clean:
	rm -f *.o

