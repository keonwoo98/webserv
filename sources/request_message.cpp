#include "request_message.hpp"

#include <sstream>

RequestMessage::RequestMessage() {}
RequestMessage::~RequestMessage() {}

void RequestMessage::SetMessage(const std::string &message) {
	std::stringstream ss(message);
	std::getline(ss, start_line_);
	size_t index = message.find("\r\n\r\n");
	headers_ =
		message.substr(start_line_.length() + 1, index - start_line_.length());
	body_ = message.substr(index + 4, message.length() - (index + 4));
}

void RequestMessage::ParsingMessage() {
	
}
