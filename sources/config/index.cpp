#include "index.hpp"

#include <sstream>

#include "config_utils.hpp"

Index::Index() {}

Index::Index(std::string &index) { index_ = Split(index, ' '); }

Index::~Index() {}

bool Index::FindIndex(const std::string &index) {
	for (size_t i = 0; i < index_.size(); i++) {
		if (index_[i] == index) return true;
	}
	return false;
}

const std::vector<std::string> &Index::GetIndex() const { return index_; }

std::string Index::ToString() const {
	std::stringstream ss;

	for (size_t i = 0; i < index_.size(); i++) {
		ss << index_[i] << ' ';
	}
	return ss.str();
}

std::ostream &operator<<(std::ostream &out, const Index &error_pages) {
	out << error_pages.ToString();
	return out;
}
