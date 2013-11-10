#include "dictionary.h"

using namespace std;
using namespace dictionary;

int Dictionary::addWord(string word){
	unordered_map<string, int>::iterator wordIter = this->wordToIndex_.find(word);
	if(wordIter == this->wordToIndex_.end()){ //word not inserted into dictionary yet
		this->index_++;		
		this->indexToWord_[index_] = word;
		//word is at position index_
		this->wordToIndex_[word] = index_;
		//add index to indexlist
		this->indexList_.push_back(index_);
		return this->index_;
	}else{ //word is in dictionary
		return wordIter->second;
	}
}

int Dictionary::getIndex(string word){
	unordered_map<string, int>::iterator wordIter = this->wordToIndex_.find(word);
	if(wordIter == this->wordToIndex_.end()){ //word not inserted into dictionary yet
		return -1;
	}else{ //word is in dictionary
		return wordIter->second;
	}
}

string Dictionary::getWord(int index){
	unordered_map<int, string>::iterator indexIter = this->indexToWord_.find(index);
	if(indexIter == this->indexToWord_.end()){
		return "";
	}else{
		return indexIter->second;
	}
}

list<int>& Dictionary::getIndices(){
	return this->indexList_;
}

int Dictionary::getNumberOfWords(){
	return this->indexList_.size();
}
