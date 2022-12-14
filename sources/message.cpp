#include "message.hpp"

Message::Message() {}
Message::~Message() {}

const std::string &Message::GetStartLine() const {
	return start_line_;
}

const std::string &Message::GetHeaders() const {
	return headers_;
}

const std::string &Message::GetBody() const {
	return body_;
}

const std::string Message::GetMessage() {
	return start_line_ + headers_ + body_;
}
