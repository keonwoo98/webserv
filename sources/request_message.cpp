#include "request_message.hpp"

RequestMessage::RequestMessage() {}
RequestMessage::~RequestMessage() {}

void RequestMessage::ParsingMessage(const std::string &message) {
	SetHeader(message);
	size_t pos = header_.find("\r\n\r\n");
	body_ = header_.substr(pos + 4, header_.size() - 1);
	header_.erase(pos + 2, header_.size() - 1);
}
