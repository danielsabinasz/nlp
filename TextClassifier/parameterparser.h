#ifndef PARAMETER_PARSER_
#define PARAMETER_PARSER_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

/*
* Parses the command lines arguments, such that they can be requested and to get their values.
*/
class ParameterParser{
	public:
		ParameterParser(char**, int);

		bool isFlagSet(std::string);
		std::string getFlagValue(std::string);

	private:
		std::vector<std::string> parameterVector_;
};

#endif
