#include "request_parser.hpp"

#include <sstream>

int RequestParser::AppendMessage(const std::string &message) {
	message_.append(message);
	switch (parsing_state_) {
		case START_LINE:
			ParsingStartLine(message);
			break;
		case HEADERS:
			ParsingHeader(message);
			break;
		case BODY:
			ParsingBody(message);
			break;
	}
}

void RequestParser::ParsingStartLine(const std::string &start_line) {
	std::stringstream ss(start_line);
	std::string buf;

	// change parsing state
	parsing_state_ = HEADERS;

	// set requeset method
	std::getline(ss, buf, ' ');
	request_.SetMethod(buf);

	// set request uri
	std::getline(ss, buf, ' ');
	request_.SetUri(buf);

	// http version check
	std::getline(ss, buf, '\r');
	if (buf != "HTTP/1.1") std::cerr << "error: " << buf << std::endl;
}

void RequestParser::ParsingHeader(const std::string &header) {
	std::string name;
	std::string value;
	size_t index;
	size_t len;

	if (header == "\r\n") {
		parsing_state_ = BODY;
		return;
	}

	index = header.find(":");
	if (index == std::string::npos ||
		header.find(":", index) != std::string::npos) {
		perror("400 Bad Request");
	}

	len = header.length();
	if (header[len - 2] != '\r' || header[len - 1] != '\n') {
		perror("400 Bad Requeset");
	}

	name = header.substr(0, index);
	value = header.substr(index + 2, header.length());
	request_.SetHeader(std::pair<std::string, std::string>(name, value));
}

void RequestParser::ParsingBody(const std::string &body) {
	request_.SetBody(body);
}
