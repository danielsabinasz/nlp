#include "countstructure.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include "parameterparser.h"
#include "dictionary.h"
#include <fstream>
#include <sstream> 
#include <cmath>
#include <limits>
#include <memory>

//#define VERBOSE

using namespace dictionary;
using namespace std;

bool readVocabulary(string, shared_ptr<Dictionary>, int);
bool readTrainingFile(string, shared_ptr<CountStructure>, shared_ptr< vector< CountStructure > >, bool, shared_ptr<Dictionary>, int&);
void writeProbabilitiesToFile(shared_ptr<CountStructure>, shared_ptr< vector< CountStructure > >, shared_ptr<Dictionary>, int&);
bool readTestFile(string, shared_ptr<CountStructure>, shared_ptr< vector< CountStructure > >, bool, shared_ptr<Dictionary>, int&);

int main(int argc, char** argv){
	//global dictionary for all words
	shared_ptr<Dictionary> dictionary(new Dictionary());
	//count structures for all categories:
	shared_ptr<vector< CountStructure > > categoryWordCount(new vector<CountStructure>());
	shared_ptr<CountStructure> categoryCount(new CountStructure());
	int totalQuantity;

	//whether the dictionary should be locked, only true is extern dictionary is used.
	bool lockedDictionary = false;

	ParameterParser parser(argv, argc);

	if((argc < 4 && !parser.isFlagSet("-h")) || (!parser.isFlagSet("-t") || !parser.isFlagSet("-d"))){
		cout << "Usage: " << argv[0] << " [-h] [-v <vocabulary file> [-n <number of words to be considered>]] -t <training data file> -d <test data file>" << endl;
		exit(1);
	}
	
	//check whether help should be displayed
	if(parser.isFlagSet("-h")){
		cout << "Help page:" << endl;
		cout << "<vocabulary file>: each line contains one word. If no vocabulary file is given, all words from the training data set will be used as vocabulary." << endl;
		cout << "<Number of words to be considered>: if a vocabulary is used only the first n words of the vocabulary file will be used, if this parameter is not used all words of the vocabulary file will be considered." << endl;
		exit(1);
	}
	//check whether vocabulary flag is set, get file name for vocabulary
	if(parser.isFlagSet("-v") && parser.getFlagValue("-v").compare("") != 0){
		string vocabularyFile = parser.getFlagValue("-v");
		cout << "Using file \"" << vocabularyFile << "\" for vocabulary." << endl;
		int n = -1;
		if(parser.isFlagSet("-n") && parser.getFlagValue("-n").compare("") != 0){			
			n = atoi(parser.getFlagValue("-n").c_str());
			cout << "Only the first " << n <<" words are taken into consideration." << endl;
		}
		//read vocabulary file and insert words into vocabulary
		readVocabulary(vocabularyFile, dictionary, n);
		lockedDictionary = true;
	}else{
		cout << "No vocabulary used." << endl;
	}

	//read training data file:
	string trainingDataFile = parser.getFlagValue("-t");
	cout << "Using file \"" << trainingDataFile << "\" for training data." << endl;
	readTrainingFile(trainingDataFile, categoryCount, categoryWordCount, lockedDictionary, dictionary, totalQuantity);

#ifdef VERBOSE
	writeProbabilitiesToFile(categoryCount, categoryWordCount, dictionary, totalQuantity);
#endif

	//read test data file:
	string testDataFile = parser.getFlagValue("-d");
	cout << "Using file \"" << testDataFile << "\" for test data." << endl;
	readTestFile(testDataFile, categoryCount, categoryWordCount, lockedDictionary, dictionary, totalQuantity);

	exit(0);
}

//read the given vocabulary file. Only these words are considered in the training and testing process
bool readVocabulary(string vocabularyFile, shared_ptr<Dictionary> dictionary, int number){
	string word;
	ifstream vocabularyFileStream(vocabularyFile);
  	if(vocabularyFileStream.is_open()){
		int i = 0;    	
		while(getline(vocabularyFileStream, word) && (number == -1 || i < number)){
      		int index = dictionary->addWord(word);
			i++;

			//cout << "Added word: " << word << ", index: " << index << endl;
		}
    	vocabularyFileStream.close();
		return true;
  	}else{
		cout << "Unable to open file"; 
		return false;
	}
}

bool readTrainingFile(string trainingDataFile, shared_ptr<CountStructure> categoryCount, shared_ptr<vector< CountStructure > > categoryWordCount, bool lockedWordDictionary, shared_ptr<Dictionary> wordDictionary, int& totalQuantity){
	string line;
	ifstream trainingDataFileStream(trainingDataFile);
  	if(trainingDataFileStream.is_open()){
		int i = 0;
		while(getline(trainingDataFileStream, line) && i < 1){
			vector<string> words;
			istringstream iss(line);
			string sub;
			while(iss >> sub){
				words.push_back(sub);
      		}
			string category = words[1];
			int index = categoryCount->addWord(category);
			//if category has not been used yet
			if(((unsigned int) index) >= categoryWordCount->size()){
				categoryWordCount->push_back(CountStructure(wordDictionary));
			}else{
				//cout << "Category \"" << category << "\" existed." << endl;
			}

			for(unsigned int i = 2; i < words.size(); i += 2){
				string word = words[i];
				int count = atoi(words[i+1].c_str());
				(*categoryWordCount)[index].addWord(word, count, lockedWordDictionary);
				//cout << "Word: " << word << ": " << count << endl;
			}
			//i++;
		}
		for(int i : categoryCount->getIndices()){
			totalQuantity += (*categoryWordCount)[i].getTotalQuantity();
		}

    	trainingDataFileStream.close();

		return true;
  	}else{
		cout << "Unable to open file"; 
		return false;
	}
}

void writeProbabilitiesToFile(shared_ptr<CountStructure> categoryCount, shared_ptr< vector< CountStructure > > categoryWordCount, shared_ptr<Dictionary> wordDictionary, int& totalQuantity){
	//write category probabilities to file:
	ofstream categoryProbabilities("catProb.txt");
	categoryProbabilities << "Probabilities for categories in relative frequencies: lenght texts of category c / total length of all texts = " << totalQuantity << endl;
	for(int i : categoryCount->getIndices()){
		categoryProbabilities << "p(\"" << categoryCount->getDictionary()->getWord(i) << "\") = " << (((long double) (*categoryWordCount)[i].getTotalQuantity() / (long double) totalQuantity)) << endl;
	}
	categoryProbabilities.close();
	cout << "Wrote probabilities for categories to file \"catProb.txt\"." << endl;

	//write word probabilities
	ofstream wordProbabilities("wordProb.txt");
	wordProbabilities << "word";
	for(int categoryIndex : categoryCount->getIndices()){
		wordProbabilities << "\t" << categoryCount->getDictionary()->getWord(categoryIndex);
	}
	for(int wordIndex : wordDictionary->getIndices()){
		wordProbabilities << "\n" << wordDictionary->getWord(wordIndex);
		for(int categoryIndex : categoryCount->getIndices()){
			wordProbabilities << "\t" << (*categoryWordCount)[categoryIndex].getProbability(wordIndex);
		}
	}
	
	wordProbabilities.close();
	cout << "Wrote probabilities for words in each category to file \"wordProb.txt\"." << endl;
}

bool readTestFile(string testDataFile, shared_ptr<CountStructure> categoryCount, shared_ptr< vector< CountStructure > > categoryWordCount, bool lockedWordDictionary, shared_ptr<Dictionary> dictionary, int& totalQuantity){
	string line;
	ifstream testDataFileStream(testDataFile);
  	if(testDataFileStream.is_open()){
		int i = 0;
		while(getline(testDataFileStream, line) && i < 1){
			vector<string> words;
			istringstream iss(line);
			string sub;
			while(iss >> sub){
				words.push_back(sub);
      		}
			string category = words[1];
			//add words and their quantities
			CountStructure wordCounts(dictionary);
			vector<int> wordIndices;
			for(unsigned int i = 2; i < words.size(); i += 2){
				string word = words[i];
				int count = atoi(words[i+1].c_str());
				int index = wordCounts.addWord(word, count, lockedWordDictionary);
				if(index != -1){
					wordIndices.push_back(index);
				}
			}
			//get true category
			int index = categoryCount->getDictionary()->getIndex(category);
			int selectedIndex = -1;
			double selectedConfidence = -numeric_limits<long double>::max();
			for(int categoryIndex : categoryCount->getIndices()){
				long double confidence = (((long double) (*categoryWordCount)[categoryIndex].getTotalQuantity() /  (long double) totalQuantity));
				for(int wordIndex : wordIndices){
					long double prob = (*categoryWordCount)[categoryIndex].getProbability(wordIndex);
					int count = wordCounts.getQuantity(wordIndex);
					//cout << "Prob: " << prob << ", Count: " << count << endl;
					
					confidence += log(prob) * count;
				}
				//cout << "Confidence for category \"" << categoryCount->getDictionary()->getWord(categoryIndex) << "\": " << confidence << endl;
				if(confidence >= selectedConfidence){
					selectedConfidence = confidence;
					selectedIndex = categoryIndex;
				}
			}
			cout << "Selected category: \"" << categoryCount->getDictionary()->getWord(selectedIndex) << "\" (confidence: " << selectedConfidence << "), correct category: \"" << categoryCount->getDictionary()->getWord(index) << "\"" << endl;
			//i++;
		}
		
    	testDataFileStream.close();
		return true;
  	}else{
		cout << "Unable to open file"; 
		return false;
	}
}
