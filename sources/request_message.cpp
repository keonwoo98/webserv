#include "request_message.hpp"

RequestMessage::RequestMessage() {}
RequestMessage::~RequestMessage() {}

void RequestMessage::SetMessage(const std::string &message) {
	size_t index;
	index = message.find("\r\n");
	start_line_ = message.substr(0, index + 2);
	index = message.find("\r\n\r\n");
	headers_ =
		message.substr(start_line_.length(), index - start_line_.length() + 2);
	body_ = message.substr(index + 4, message.length() - (index + 4));
}

void RequestMessage::ParsingMessage() {
	
}
