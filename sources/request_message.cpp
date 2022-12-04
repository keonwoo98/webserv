#include "request_message.hpp"
#include <sstream>

RequestMessage::RequestMessage() {}
RequestMessage::~RequestMessage() {}

void RequestMessage::ParsingMessage(const std::string &message) {
	std::string buf;
	size_t index;

	index = message.find("\r\n");
	buf = message.substr(0, index + 1);
	ParsingStartLine(buf);

	index = message.find("\r\n\r\n");
	buf = message.substr(buf.length(), index- buf.length() + 2);
	ParsingHeaders(buf);

	body_ = message.substr(index + 4, message.length() - (index + 4));
}

void RequestMessage::ParsingStartLine(const std::string start_line) {
	std::stringstream ss(start_line);
	std::string buf;

	std::getline(ss, method_, ' ');
	if (method_ != "GET" && method_ != "POST" && method_ != "DELETE")
		std::cerr << "error: " << method_ << std::endl;

	std::getline(ss, uri_, ' ');

	std::getline(ss, buf, 'R');
	if (buf != "HTTP/1.1")
		std::cerr << "error: " << buf << std::endl;
}

void RequestMessage::SplitHeader(const std::string &header) {
	std::string key;
	std::string value;
	size_t index;

	index = header.find(": ");
	key = header.substr(0, index);
	value = header.substr(index + 2, header.length());
	header_map_[key] = value;
}

void RequestMessage::ParsingHeaders(const std::string headers) {
	size_t start = 0;
	size_t end;

	end = headers.find("\r\n");
	size_t i = 0;
	while (end != std::string::npos) {
		SplitHeader(headers.substr(start, end - start));
		start = end + 2;
		end = headers.find("\r\n", start);
	}
}
