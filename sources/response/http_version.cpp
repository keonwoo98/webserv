//
// Created by 김민준 on 2022/12/19.
//

#include "http_version.hpp"

#include <sstream>

HttpVersion::HttpVersion(int major, int minor) : major_(major), minor_(minor) {}

std::string HttpVersion::ToString() const {
	std::stringstream ss;

	ss << "HTTP/" << major_ << "." << minor_;
	return ss.str();
}

std::ostream &operator<<(std::ostream &out, HttpVersion http_version) {
	out << http_version.ToString();
	return out;
}
