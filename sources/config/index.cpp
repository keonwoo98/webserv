#include "index.hpp"
#include "config_utils.hpp"

Index::Index() {}

Index::Index(std::string &index) {
	index_ = Split(index, ' ');
}

Index::~Index() {}

bool Index::FindIndex(const std::string &index) {
	for (size_t i = 0; i < index_.size(); i++) {
		if (index_[i] == index)
			return true;
	}
	return false;
}

const std::vector<std::string> &Index::GetIndex() const {
	return index_;
}
