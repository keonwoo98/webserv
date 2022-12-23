#include "config_utils.hpp"

#include <sstream>

std::vector<std::string> Split(std::string input, char delimiter) {
	std::vector<std::string> str;
	std::stringstream ss(input);
	std::string temp;

	while (std::getline(ss, temp, delimiter)) str.push_back(temp);

	return str;
}
