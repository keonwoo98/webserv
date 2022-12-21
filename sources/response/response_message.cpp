#include "response_message.hpp"
#include <string>
#include <sstream>

std::string ResponseMessage::ToString() {
	std::stringstream ss;

	ss << status_line_ << headers_ << body_;
	return ss.str();
}

ResponseMessage::ResponseMessage(int status_code, const std::string &reason_phrase)
	: status_line_(StatusLine(HttpVersion(), status_code, reason_phrase)) {
	headers_.AddServer();
}

void ResponseMessage::SetBody(const std::string &body) {
	headers_.AddContentLength(body);
	body_ = body;
}

std::ostream &operator<<(std::ostream &out, ResponseMessage message) {
	out << message.ToString();
	return out;
}