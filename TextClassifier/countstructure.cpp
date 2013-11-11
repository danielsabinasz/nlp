#include "countstructure.h"

#include <iostream>

using namespace std;
using namespace dictionary;

CountStructure::CountStructure(){
	this->dictionary_ = shared_ptr<Dictionary>(new Dictionary());
}

CountStructure::CountStructure(shared_ptr<Dictionary> dictionary){
	this->dictionary_ = dictionary;
}

int CountStructure::addWord(string word){
	this->isOrdered_ = false;
	int result = dictionary_->addWord(word);
	increaseQuantity(result);
	this->text_.push_back(result);
	return result;
}

//parameter locked: if a vocabulary has been used: do not add words if locked flag is set.
//returns -1 if word could not be add to dictionary
int CountStructure::addWord(string word, bool locked){
	if(locked == true && dictionary_->getIndex(word) == -1){	
		return -1;
	}
	return addWord(word);
}

int CountStructure::addWord(string word, int quantity){
	this->isOrdered_ = false;
	int result = dictionary_->addWord(word);
	increaseQuantity(result, quantity);
	this->text_.push_back(result);
	return result;
}

//parameter locked: if a vocabulary has been used: do not add words if locked flag is set.
//returns -1 if word could not be add to dictionary
int CountStructure::addWord(string word, int quantity, bool locked){
	if(locked == true && dictionary_->getIndex(word) == -1){
		return -1;
	}
	return addWord(word, quantity);
}

int CountStructure::increaseQuantity(int index, int quantity){
	this->totalQuantity_ += quantity;
	unordered_map<int, int>::iterator it = quantities_.find(index);
	if(it == quantities_.end()){ //index not inserted yet
		quantities_[index] = quantity;
		return 1;
	}else{
		it->second += quantity;
		int quantity = it->second;
		return quantity;
	}
}

int CountStructure::increaseQuantity(int index){
	this->totalQuantity_++;
	unordered_map<int, int>::iterator it = quantities_.find(index);
	if(it == quantities_.end()){ //index not inserted yet
		quantities_[index] = 1;
		return 1;
	}else{
		int quantity = ++(it->second);
		return quantity;
	}
}

int CountStructure::getQuantity(int index){
	unordered_map<int, int>::iterator it = quantities_.find(index);
	if(it == this->quantities_.end()){ //index not found
		return 0;
	}else{
		return it->second;
	}
}

list<int>& CountStructure::getIndices(){
	return dictionary_->getIndices();	
}

list<int>& CountStructure::getOrderedIndices(){
	if(!this->isOrdered_){
		ordered_ = dictionary_->getIndices();
		ordered_.sort(
			[&](int index1, int index2){ 
				return (getQuantity(index1) >= getQuantity(index2));
			}
		);
		isOrdered_ = true;
	}
	return ordered_;
}

shared_ptr<Dictionary> CountStructure::getDictionary(){
	return this->dictionary_;
}

vector<int>& CountStructure::getText(){
	return this->text_;
}

int CountStructure::getTotalQuantity(){
	return this->totalQuantity_;
}

long double CountStructure::getProbability(int index){
	return ((long double)this->getQuantity(index) / (long double)this->getTotalQuantity());
}

void CountStructure::applyAbsoluteSmooting(){
	for(int index : dictionary_->getIndices()){
		increaseQuantity(index);
	}
}
