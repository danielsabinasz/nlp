#include "dictionary.hh"
#include <iostream>

using namespace std;

dictionary::dictionary() {
}

//Insert or lookup a word.
unsigned int dictionary::id(string key) {
    if (words_to_ids.count(key) == 0) { //If no entry present insert
        words_to_ids[key] = ids_to_words.size();
        ids_to_words.push_back(key);
        return ids_to_words.size()-1;
    } else {
        return words_to_ids[key]; //Else return key.
    }
}

//Lookup word
string dictionary::word(unsigned int key) {
    return ids_to_words.at(key);
}

