//
// Created by 김민준 on 2022/11/30.
//

#include "message/http_version.hpp"

#include <sstream>

HttpVersion::HttpVersion(int major, int minor) : major_(major), minor_(minor) {}

HttpVersion::HttpVersion(const HttpVersion &rhs) {
	this->major_ = rhs.major_;
	this->minor_ = rhs.minor_;
}

HttpVersion &HttpVersion::operator=(const HttpVersion &rhs) {
	this->major_ = rhs.major_;
	this->minor_ = rhs.minor_;
	return *this;
}

HttpVersion::~HttpVersion() {}

std::string HttpVersion::toString() const {
	std::stringstream ss;
	ss << "HTTP/" << major_ << "." << minor_;
	return ss.str();
}

std::ostream &operator<<(std::ostream &os, const HttpVersion &version) {
	os << version.toString();
	return os;
}
