#include "message/response_message.hpp"

#include <sstream>

ResponseMessage::ResponseMessage(const StatusLine &status_line, const Header &header, const Body &body) : status_line(
	status_line), header_(header), body_(body) {}

ResponseMessage::~ResponseMessage() {}

std::string ResponseMessage::toString() const {
	std::stringstream ss;
	ss << status_line << header_ << body_;
	return ss.str();
}

std::ostream &operator<<(std::ostream &os, const ResponseMessage &message) {
	os << message.toString();
	return os;
}
