CXX=g++
CXXFLAGS=-Wall -O2 -std=c++11
all: trainer classifier

trainer: dictionary.o trainer.cc
classifier: dictionary.o classifier.cc
dictionary.o: dictionary.cc dictionary.hh

.PHONY: clean veryclean
clean:
	rm -f *.o

