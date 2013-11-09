#ifndef __LEXICON_H__
#define __LEXICON_H__

#include <string>
#include <map>
#include <vector>
using namespace std;

class dictionary {
    private:
        map<string, unsigned int> words_to_ids;
	vector<string> ids_to_words;

    public:
        dictionary();
        unsigned int id(string key);
        string word(unsigned int key);
	unsigned int getWordCount();
};

#endif

