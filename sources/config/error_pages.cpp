#include "error_pages.hpp"

#include <sstream>

#include "config_utils.hpp"

ErrorPages::ErrorPages() {}

ErrorPages::~ErrorPages() {}

void ErrorPages::Append(std::string &error_pages) {
	std::vector<std::string> temp = Split(error_pages, ' ');
	std::string path = temp[temp.size() - 1];
	for (size_t i = 0; i < temp.size() - 1; i++) {
		error_pages_.insert(
			std::pair<int, std::string>(atoi(temp[i].c_str()), path));
	}
}

const std::string ErrorPages::GetPath(int &num) const {
	for (error_page_type::const_iterator it = error_pages_.begin();
		 it != error_pages_.end(); it++) {
		if (it->first == num) return it->second;
	}
	return "";
}

const ErrorPages::error_page_type &ErrorPages::GetErrorPages() const {
	return error_pages_;
}

std::string ErrorPages::ToString() const {
	std::stringstream ss;

	for (error_page_type::const_iterator it = error_pages_.begin();
		 it != error_pages_.end(); it++) {
		ss << "      \t" << it->first << ' ' << it->second << '\n';
	}
	return ss.str();
}

std::ostream &operator<<(std::ostream &out, const ErrorPages &error_pages) {
	out << error_pages.ToString();
	return out;
}
