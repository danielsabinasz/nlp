#include "countstructure.h"
#include "parameterparser.h"
#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <cmath>
#include <limits>
#include <memory>

//#define VERBOSE
#define SMOOTHING

using namespace dictionary;
using namespace std;

//read vocabulary file
bool readVocabulary(string, shared_ptr<Dictionary>, int);

//read training datafile, compute relative frequencies
bool readTrainingFile(string, shared_ptr<CountStructure>, shared_ptr< vector< CountStructure > >, bool, shared_ptr<Dictionary>);

//write computed probabilities into file
void writeProbabilitiesToFile(shared_ptr<CountStructure>, shared_ptr< vector< CountStructure > >, shared_ptr<Dictionary>);

//read words from test file, used for smooting!
bool readTestFile(string, shared_ptr<CountStructure>, shared_ptr< vector< CountStructure > >, bool, shared_ptr<Dictionary>);

//categorize texts from test file, prints selected category, confidence for category and the true category
bool categorizationOfTestFile(string, shared_ptr<CountStructure>, shared_ptr< vector< CountStructure > >, bool, shared_ptr<Dictionary>);

int main(int argc, char** argv){

	//global dictionary for all words
	shared_ptr<Dictionary> dictionary(new Dictionary());
	//count structures for all categories:
	shared_ptr<vector< CountStructure > > categoryWordCount(new vector<CountStructure>());
	//count structure to get quantities for each category (occurences of categories in training data set)
	shared_ptr<CountStructure> categoryCount(new CountStructure());
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
	readTrainingFile(trainingDataFile, categoryCount, categoryWordCount, lockedDictionary, dictionary);
	
	//read test data file:
	string testDataFile = parser.getFlagValue("-d");
	cout << "Using file \"" << testDataFile << "\" for test data." << endl;
	readTestFile(testDataFile, categoryCount, categoryWordCount, lockedDictionary, dictionary);
	
#ifdef VERBOSE
	writeProbabilitiesToFile(categoryCount, categoryWordCount, dictionary);
#endif

	//categorization of test data file:
	cout << "Categorizing texts from file \"" << testDataFile << "\"." << endl;
	categorizationOfTestFile(testDataFile, categoryCount, categoryWordCount, lockedDictionary, dictionary);

	//print information about the dictionary:
	cout << "Dictionary-size: " << dictionary->getIndices().size() << endl;
	exit(0);
}

//read the given vocabulary file. Only these words are considered in the training and testing process
bool readVocabulary(string vocabularyFile, shared_ptr<Dictionary> dictionary, int number){
	string word;
	ifstream vocabularyFileStream(vocabularyFile);
  	if(vocabularyFileStream.is_open()){
		int i = 0;    	
		while(getline(vocabularyFileStream, word) && (number == -1 || i < number)){
      		/*int index = */dictionary->addWord(word);
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

bool readTrainingFile(string trainingDataFile, shared_ptr<CountStructure> categoryCount, shared_ptr<vector< CountStructure > > categoryWordCount, bool lockedWordDictionary, shared_ptr<Dictionary> wordDictionary){
	string line;
	ifstream trainingDataFileStream(trainingDataFile);
  	if(trainingDataFileStream.is_open()){
		while(getline(trainingDataFileStream, line)){
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
		}
		
    	trainingDataFileStream.close();

		return true;
  	}else{
		cout << "Unable to open file"; 
		return false;
	}
}

void writeProbabilitiesToFile(shared_ptr<CountStructure> categoryCount, shared_ptr< vector< CountStructure > > categoryWordCount, shared_ptr<Dictionary> wordDictionary){
	//write category probabilities to file:
	ofstream categoryProbabilities("catProb.txt");
	categoryProbabilities << "Probabilities for categories in relative frequencies: #occurences of category c / #all texts in training data" << endl;
	for(int i : categoryCount->getIndices()){
		categoryProbabilities << "p(\"" << categoryCount->getDictionary()->getWord(i) << "\") = " << categoryCount->getProbability(i) << endl;
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

bool readTestFile(string testDataFile, shared_ptr<CountStructure> categoryCount, shared_ptr< vector< CountStructure > > categoryWordCount, bool lockedWordDictionary, shared_ptr<Dictionary> dictionary){
	string line;
	ifstream testDataFileStream(testDataFile);
  	if(testDataFileStream.is_open()){
		while(getline(testDataFileStream, line)){
			vector<string> words;
			istringstream iss(line);
			string sub;
			while(iss >> sub){
				words.push_back(sub);
      		}
			
			for(unsigned int i = 2; i < words.size(); i += 2){
				string word = words[i];
				if(lockedWordDictionary == false){
					dictionary->addWord(word);
				}
			}
		}

#ifdef SMOOTHING
		cout << "Apply smooting: prob(w | c) = (N(w,c)+1 / sum of all N(w', c) per category" << endl;
		for(CountStructure& cStruct : *categoryWordCount){
			cStruct.applyAbsoluteSmooting();
		}
#endif

		return true;
  	}else{
		cout << "Unable to open file"; 
		return false;
	}
}

bool categorizationOfTestFile(string testDataFile, shared_ptr<CountStructure> categoryCount, shared_ptr< vector< CountStructure > > categoryWordCount, bool lockedWordDictionary, shared_ptr<Dictionary> dictionary){
#ifdef VERBOSE
	ofstream categoryAssignmentFile("result.txt");
#endif
	string line;
	ifstream testDataFileStream(testDataFile);
  	if(testDataFileStream.is_open()){
		int errorCount = 0;
		int error = 0;
		int textCount = 0;
		while(getline(testDataFileStream, line)){
			textCount++;
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

			//get true category for actual text
			int index = categoryCount->getDictionary()->getIndex(category);
			int selectedIndex = -1;
			double selectedConfidence = -numeric_limits<long double>::max();
			for(int categoryIndex : categoryCount->getIndices()){
				long double wordsInClass = (long double) (*categoryWordCount)[categoryIndex].getTotalQuantity();
				long double confidence = log(categoryCount->getProbability(categoryIndex));
				
				for(int wordIndex : wordIndices){
					long double wordQuantity = (long double) (*categoryWordCount)[categoryIndex].getQuantity(wordIndex);
					//long double logProb = log(wordQuantity) - log(wordsInClass);
					long double logProb = log(wordQuantity / wordsInClass);
					int count = wordCounts.getQuantity(wordIndex);
					
					confidence += logProb * count;
				}
				if(confidence > selectedConfidence){
					selectedConfidence = confidence;
					selectedIndex = categoryIndex;
				}
			}
			cout << "Selected category: \"" << categoryCount->getDictionary()->getWord(selectedIndex) << "\" (confidence: " << selectedConfidence << "), correct category: \"" << categoryCount->getDictionary()->getWord(index) << "\"" << endl;

#ifdef VERBOSE
			//write result of assignment to file
			categoryAssignmentFile << "Selected category: \"" << categoryCount->getDictionary()->getWord(selectedIndex) << "\" (confidence: " << selectedConfidence << "), correct category: \"" << categoryCount->getDictionary()->getWord(index) << "\"" << endl;
#endif

			if(selectedIndex != index){
				errorCount++;
			}
			if(selectedIndex == -1){
				error++;
			}
		}
		
		//print number of errors in classification:
		cout << "False category assigned in: " << errorCount << " cases (" << textCount << " texts)." << endl;
		cout << "No category assigned in: " << error << " cases." << endl;
#ifdef VERBOSE
		//write error counts to result file
		categoryAssignmentFile << "False category assigned in: " << errorCount << " cases (" << textCount << " texts)." << endl;
		categoryAssignmentFile << "No category assigned in: " << error << " cases." << endl;
		categoryAssignmentFile.close();
#endif

    	testDataFileStream.close();
		
		return true;
  	}else{
		cout << "Unable to open file"; 
		return false;
	}
}
