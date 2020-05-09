/*
 * LocalizeVMT
 * MIT License
 * Copyright (c) 2020 polygonerror
 *
 */

#include "utility.hpp"

std::vector<std::string> Utility::split(std::string pString, std::string pDelimit) {
	size_t pos = 0;
	std::vector<std::string> tokens;
	while ((pos = pString.find(pDelimit)) != std::string::npos) {
		tokens.push_back(pString.substr(0, pos));
		pString.erase(0, pos + 1);
	}
	tokens.push_back(pString.substr(0, pos)); //push last token

	return tokens;
}
