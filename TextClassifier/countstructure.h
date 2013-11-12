#ifndef COUNTSTRUCTURE_H
#define COUNTSTRUCTURE_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include "dictionary.h"
#include <memory>

namespace dictionary{

/*
* A CountStructure stores the quantites for words in a given category. If a word is add which has not been
* contained in the Dictionary of this CountStructure the word will be add if it is allowed to. This means: 
* if the boolean flag is true, the dictionary of this structure is locked and no further word will be add!
* This includes that the quantities of the words that are not contained within the dictionary are not
* considered when computing the probabilities for each word.
* In textclassification for each class one separate CountStructure is used.
*/
class CountStructure{
	public:
		/*
		* Constructor, dictionary of CountStructure is the given dictionary.
		*/
		CountStructure(std::shared_ptr<Dictionary>);

		/*
		* Constructor, empty dictionary!
		*/
		CountStructure();

		/*
		* Returns quantity for a word with given index.
		* returns 0 if word is not part of the dictionary.
		*/
		int getQuantity(int);

		/*
		* Adds a word to the CountStructure. Returns the index of the word in the dictionary.
		* Increases the quantity by 1.
		*/
		int addWord(std::string);
		
		/*
		* Adds a word to the CountStructure. Returns the index of the word in the dictionary.
		* Increases the quantity by 1.
		* If a vocabulary has been used: do not add words if locked flag is set.
		* returns -1 if word could not be add to dictionary
		*/
		int addWord(std::string, bool);

		/*
		* Adds a word to the CountStructure. Returns the index of the word in the dictionary.
		* Increases the quantity by n (given int parameter).
		*/
		int addWord(std::string, int);

		/*
		* Adds a word to the CountStructure. Returns the index of the word in the dictionary.
		* Increases the quantity by n (given int parameter).
		* If a vocabulary has been used: do not add words if locked flag is set.
		* returns -1 if word could not be add to dictionary
		*/
		int addWord(std::string, int, bool);

		/*
		* Returns a list of all indices that are contained in the dictionary of this CountStructure.
		*/
		std::list<int>& getIndices();

		/*
		* Returns a list of all indices that are contained in the dictionary of this CountStructure.
		* The indices are ordered by their quantity!
		*/
		std::list<int>& getOrderedIndices();

		/*
		* Returns probability of the word with given index. p(w|c) = N(c,w) / sum_{w'} N(c,w')
		* returns 0 if word is not contained!
		*/
		long double getProbability(int);

		std::vector<int>& getText();
		
		/*
		* Returns sum over all word quantities.
		* = sum{w'} N(c,w')
		*/
		int getTotalQuantity();

		std::shared_ptr<Dictionary> getDictionary();

		/*
		* Smooting: adds 1 to each words (quantity + 1).
		*/
		void applyAbsoluteSmooting();

	private:
		int increaseQuantity(int);
		int increaseQuantity(int, int);
		std::shared_ptr<Dictionary> dictionary_;
		std::vector<int> text_;
		std::unordered_map<int, int> quantities_;
		std::list<int> ordered_;
		bool isOrdered_ = true;
		int totalQuantity_ = 0;
};

}

#endif
