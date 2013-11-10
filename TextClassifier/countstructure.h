#ifndef COUNTSTRUCTURE_H
#define COUNTSTRUCTURE_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include "dictionary.h"
#include <memory>

namespace dictionary{

class CountStructure{
	public:
		CountStructure(std::shared_ptr<Dictionary>);
		CountStructure();

		int getQuantity(int);
		int addWord(std::string);
		int addWord(std::string, bool);
		int addWord(std::string, int);
		int addWord(std::string, int, bool);
		std::list<int>& getIndices();
		std::list<int>& getOrderedIndices();

		long double getProbability(int);

		std::vector<int>& getText();
		
		int getTotalQuantity();

		std::shared_ptr<Dictionary> getDictionary();

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
