#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <tuple>
#include <algorithm>
#include <unordered_set>
#include <math.h>

#include "dictionary.hh"

using namespace std;

int main(int argc, char **argv) {
	if (argc < 2) {
		cout << "usage: " << argv[0]
		<< " <training set filename>"
		<< " [<vocabulary filename>]"
		<< endl;
		return EXIT_FAILURE;
	}

	// Check training set file
	char *training_set_filename = argv[1];
	ifstream f_train(training_set_filename);
	if (!f_train.good()) {
		cerr << "ERROR: Opening file '"
		<< training_set_filename << "' failed."
		<< endl;
		return EXIT_FAILURE;
	}

	// If a vocabulary has been specified, load it
	bool restrict_vocabulary = false;
	unordered_set<string> restrict_vocabulary_set;
	if (argc >= 3) {
		restrict_vocabulary = true;

		// Check vocabulary file
		char *vocabulary_filename = argv[2];
		ifstream f_voc(vocabulary_filename);
		if (!f_voc.good()) {
			cerr << "ERROR: Opening file '"
			<< vocabulary_filename << "' failed."
			<< endl;
			return EXIT_FAILURE;
		}

		// Read file line by line
		string word;
		while (getline(f_voc, word)) {
			restrict_vocabulary_set.insert(word);
		}
	}

	// Dictionary
	dictionary *dict = new dictionary();

	// Track class-wide word counts
	// (map class names to a map that maps word ids to counts)
	typedef map<unsigned int, unsigned int> word_counts_map;
	typedef map<string, word_counts_map> class_word_counts_map;
	class_word_counts_map word_counts;

	// Track total word count per class
	typedef map<string, unsigned int> class_total_word_counts_map;
	class_total_word_counts_map total_word_counts;

	// Iterate file line by line (each line represents a document)
	string document;
	while (getline(f_train, document)) {
		// Tokenize the document
		istringstream iss(document);
		
		// Document name
		string document_name;
		iss >> document_name;

		// Document class
		string document_class;
		iss >> document_class;

		// Document length (N)
		unsigned int document_length = 0;

		string word;
		unsigned int word_count;
		while (iss >> word) {
			// Word count
			iss >> word_count;

			// If a vocabulary has been specified, check whether this word is in it
			if (restrict_vocabulary && restrict_vocabulary_set.count(word) == 0) continue;

			// Get word id
			unsigned int word_id = dict->id(word);

			// Increase document length
			document_length += word_count;

			// Increase class-wide word count for this word
			if (word_counts[document_class].count(word_id) == 0) {
				word_counts[document_class][word_id] = 0;
			}
			word_counts[document_class][word_id]++;

			// Increase class-wide total word count
			if (total_word_counts.count(document_class) == 0) {
				total_word_counts[document_class] = 0;
			}
			total_word_counts[document_class]++;
		}
	}

	// Iterate all classes
	for (class_word_counts_map::iterator it = word_counts.begin(); it != word_counts.end(); it++) {
		string class_name = it->first;
		int total_word_count = total_word_counts[class_name];

		cout << class_name << ":" << endl;

		// Iterate all words for that class
		for (word_counts_map::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			int word_id = it2->first;
			int word_count = it2->second;

			// Determine relative frequency of that word in the class
			// (stored as negative logarithm for numeric stability;
			// this is the same as -log(word_count/total_word_count)
			double rel_freq = log(total_word_count) - log(word_count);

			cout << dict->word(word_id) << " " << rel_freq << endl;
		}

		cout << endl;
	}

}

