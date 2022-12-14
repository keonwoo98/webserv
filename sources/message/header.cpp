#include "message/header.hpp"

#include <sstream>

Header::Header() {}

Header::~Header() {}

std::string Header::toString() const {
	std::stringstream ss;

	std::map<std::string, std::vector<std::string> >::const_iterator it_map;
	for (it_map = headers_.begin(); it_map != headers_.end(); it_map++) {
		std::string key = it_map->first;
		ss << key << ": ";
		std::vector<std::string> value = it_map->second;
		std::vector<std::string>::const_iterator it_vector;
		for (it_vector = value.begin(); it_vector != value.end(); it_vector++) {
			const std::string &field_value = *it_vector;
			ss << field_value;
			if (it_vector + 1 != value.end()) {
				ss << ',';
			}
		}
		ss << "\r\n";
	}
	ss << "\r\n";
	return ss.str();
}

std::ostream &operator<<(std::ostream &os, const Header &header) {
	os << header.toString();
	return os;
}
void Header::Add(std::string &key, std::vector<std::string> &value) {
	headers_[key] = value;
}

