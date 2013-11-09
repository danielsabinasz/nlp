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
	if (argc != 3) {
		cout << "usage: " << argv[0]
		<< " <filename>"
		<< " <n>"
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

	// n most frequent words
	unsigned int n = atoi(argv[2]);
	
	// Vector containing tuple of frequency and word id
	vector<pair<unsigned int, unsigned int>> *freq = new vector<pair<unsigned int, unsigned int>>();

	// dictionary
	dictionary *dict = new dictionary();

	// Iterate file line by line
	string line;
	while (getline(f, line)) {
		// Iterate line word by word
		istringstream iss(line);
		string word;
		while (iss >> word) {
			// Word to lower
			string word_lower = word;
			transform(word_lower.begin(), word_lower.end(), word_lower.begin(), ::tolower);

			// Get word id
			unsigned int word_id = dict->id(word_lower);

			// If the word is new, add frequency counter
			if (word_id >= freq->size()) {
				freq->push_back(pair<unsigned int, unsigned int>(0, word_id));
			}

			// Increase frequency counter
			freq->at(word_id).first += 1;

		}
	}

	// Sort frequencies
	sort(freq->rbegin(), freq->rend());

	// Unordered set holding the n most frequent words
	unordered_set<unsigned int> *most_freq = new unordered_set<unsigned int>();

	// Determine n most frequent words
	vector<pair<unsigned int, unsigned int>>::iterator it;
	unsigned int i = 0;
	for (it = freq->begin(); i < n && it != freq->end(); i++, it++) {
		most_freq->insert(it->second);
	}

	// Iterate file again
	f.clear();
	f.seekg(0);
	while (getline(f, line)) {
		istringstream iss(line);
		string word;
		while (iss >> word) {
			// Word to lower
			string word_lower = word;
			transform(word_lower.begin(), word_lower.end(), word_lower.begin(), ::tolower);

			unsigned int word_id = dict->id(word_lower);

			if (most_freq->count(word_id) == 0) cout << "<UNK>";
			else cout << word;
			cout << " ";
		}
		cout << endl;
	}
}

