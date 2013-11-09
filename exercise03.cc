#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <tuple>
#include <algorithm>
#include <unordered_set>

#include "dictionary.hh"

using namespace std;

int main(int argc, char **argv) {
	if (argc != 2) {
		cout << "usage: " << argv[0]
		<< " <training set file>"
		<< endl;
		return EXIT_FAILURE;
	}

	// Check file
	char *filename = argv[1];
	ifstream f(filename);
	if (!f.good()) {
		cerr << "ERROR: Opening file '"
		<< filename << "' failed."
		<< endl;
		return EXIT_FAILURE;
	}

	// dictionary
	dictionary *dict = new dictionary();

	// Iterate file line by line (each line represents a document)
	string document;
	while (getline(f, document)) {
		// Tokenize the document
		istringstream iss(document);
		
		// Document name
		string document_name;
		iss >> document_name;

		// Document class
		string document_class;
		iss >> document_class;

		string word;
		int word_count;
		while (iss >> word) {
			// Get word id
			unsigned int word_id = dict->id(word);

			// Word count
			iss >> word_count;
		}
	}

}

