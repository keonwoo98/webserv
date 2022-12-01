/**
 * status-line = HTTP-version SP status-code SP reason-phrase CRLF
 */

#include "message/status_line.hpp"

#include <sstream>

StatusLine::StatusLine(HttpVersion version, int status_code, std::string reason_phrase)
	: version_(version), status_code_(status_code), reason_phrase_(reason_phrase) {}

StatusLine::~StatusLine() {}

std::ostream &operator<<(std::ostream &os, const StatusLine &status_line) {
	os << status_line.toString();
	return os;
}

std::string StatusLine::toString() const {
	std::stringstream ss;
	ss << version_ << ' ' << status_code_ << ' ' << reason_phrase_ << "\r\n";
	return ss.str();
}

