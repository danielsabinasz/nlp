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
		<< " <training set filename (.tr)>"
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
	class_word_counts_map class_word_counts;

	// Track total word count per class
	typedef map<string, unsigned int> class_total_word_counts_map;
	class_total_word_counts_map class_total_word_counts;

	// Track total document count per class (for our p(c) model)
	typedef map<string, unsigned int> class_document_count_map;
	class_document_count_map class_document_count;

	// Track total document count (for our p(c) model)
	unsigned int total_document_count = 0;

	// For our Poisson P(N|c) model, track the sum of the document lengths per class.
	// This sum will later be divided by the number of documents in that class,
	// yielding the maximum likelihood estimate for the Poisson parameter
	typedef map<string, unsigned int> class_document_length_sum_map;
	class_document_length_sum_map class_document_length_sum;

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

		// Increase document count for this class
		if (class_document_count.count(document_class) == 0) {
			class_document_count[document_class] = 0;
		}
		class_document_count[document_class]++;

		// Increase total document count
		total_document_count++;

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
			if (class_word_counts[document_class].count(word_id) == 0) {
				class_word_counts[document_class][word_id] = 0;
			}
			class_word_counts[document_class][word_id]++;

			// Increase class-wide total word count
			if (class_total_word_counts.count(document_class) == 0) {
				class_total_word_counts[document_class] = 0;
			}
			class_total_word_counts[document_class]++;
		}

		// Increase sum of document lengths for the class
		if (class_document_length_sum.count(document_class) == 0) {
			class_document_length_sum[document_class] = 0;
		}
		class_document_length_sum[document_class] += document_length;
	}

	// Iterate all classes
	for (class_word_counts_map::iterator it = class_word_counts.begin(); it != class_word_counts.end(); it++) {
		string class_name = it->first;

		// Determine relative frequency of the class in total (for our p(c) model)
		// (stored as negative logarithm for numeric stability;
		// this is the same as -log(document_count/total_document_count)
		double class_freq = log(total_document_count) - log(class_document_count[class_name]);

		// Determine average length for documents of this class (i.e. the Poisson parameter for the P(N|c) model)
		double poisson_parameter = class_document_length_sum[class_name]/class_document_count[class_name];

		// Read total word count from map
		int total_word_count = class_total_word_counts[class_name];

		// Output class name, estimate for p(c) and Poisson parameter for p(N|c)
		cout << class_name << " " << class_freq << " " << poisson_parameter << " ";

		// Iterate all words for that class
		for (word_counts_map::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			int word_id = it2->first;
			int word_count = it2->second;

			// Determine relative frequency of that word in the class
			double word_freq = log(total_word_count) - log(word_count);

			cout << dict->word(word_id) << " " << word_freq << " ";
		}

		cout << endl;
	}

}

