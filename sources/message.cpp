#include "message.hpp"

Message::Message() {}
Message::~Message() {}

void Message::SetHeader(const std::string &header) {
	header_ = header;
}

void Message::SetBody(const std::string &body) {
	body_ = body;
}

const std::string &Message::GetHeader() const {
	return header_;
}

const std::string &Message::GetBody() const {
	return body_;
}
