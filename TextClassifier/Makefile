CXX=g++
CXXFLAGS=-Wall -O2 -std=c++0x

all: textclassifier

textclassifier: parameterparser.o countstructure.o dictionary.o textclassifier.cpp
countstructure.o: dictionary.o countstructure.cpp countstructure.h
dictionary.o: dictionary.cpp dictionary.h
parameterparser.o: parameterparser.cpp parameterparser.h

.PHONY: clean veryclean
clean:
	rm -f *.o

veryclean: clean
	rm -f *~
