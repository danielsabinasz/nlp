#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <memory>
#include <list>

namespace dictionary{

/**
* Class Dictionary provides a mechanism to store words. For each inserted word the index within the dictionary 
* is returned. It is also possible to get the word for a given index.
*/
class Dictionary{
	public:
		
		//add word a word to the dictionary, the index is returned. If the word is in the dictionary yet, the index is returned.
		int addWord(std::string);
		
		//get index for a word, if not in dictionary, result is -1
		int getIndex(std::string);
		
		//get word for index, if index is not assigned to a word: NULL is returned
		std::string getWord(int);

		//get list of indices
		std::list<int>& getIndices();

		//get number of words stored in dictionary
		int getNumberOfWords();
	
	private:
		std::unordered_map<int, std::string> indexToWord_;
		std::unordered_map<std::string, int> wordToIndex_;
		std::list<int> indexList_;
		int index_ = -1;
};

}

#endif
