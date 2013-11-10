#include "parameterparser.h"

using namespace std;

ParameterParser::ParameterParser(char** argv, int argc){
	for(int i = 1; i < argc; i++){
		this->parameterVector_.push_back(string(argv[i]));
	}
}

bool ParameterParser::isFlagSet(string flag){
	for(unsigned int i = 0; i < this->parameterVector_.size(); i++){
		if(this->parameterVector_[i].compare(flag) == 0){
			return true;
		}
	}
	return false;
}	

string ParameterParser::getFlagValue(string flag){
	for(unsigned int i = 0; i < this->parameterVector_.size(); i++){
		if(this->parameterVector_[i].compare(flag) == 0){
			if((i+1) < this->parameterVector_.size()){
				return parameterVector_[(i+1)];	
			}else{
				return "";
			}
		}
	}
	return "";
}
