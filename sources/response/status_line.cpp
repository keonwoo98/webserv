//
// Created by 김민준 on 2022/12/19.
//

#include "status_line.hpp"
#include "character_const.hpp"

#include <sstream>

StatusLine::StatusLine(const HttpVersion &http_version, int status_code, const std::string &reason_phrase)
	: http_version_(http_version), status_code_(status_code), reason_phrase_(reason_phrase) {
}

StatusLine::StatusLine() : http_version_(HttpVersion()), status_code_(0), reason_phrase_("") {
}

void StatusLine::Clear() {
	status_code_ = 0;
	reason_phrase_.clear();
}

int StatusLine::GetStatusCode() {
	return status_code_;
}

void StatusLine::SetStatus(int status_code, const std::string &reason_phrase) {
	status_code_ = status_code;
	reason_phrase_ = reason_phrase;
}

std::string StatusLine::ToString() {
	std::stringstream ss;

	ss << http_version_ << " " << status_code_ << " " << reason_phrase_ << CRLF;
	return ss.str();
}

std::ostream &operator<<(std::ostream &out, StatusLine status_line) {
	out << status_line.ToString();
	return out;
}
