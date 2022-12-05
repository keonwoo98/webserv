#include "request_message.hpp"
#include <sstream>
#include <fstream>

RequestMessage::RequestMessage() {
	std::string filePath = "ex/request-headers";

	std::ifstream openFile(filePath.data());
	if (openFile.is_open()) {
		std::string line;
		while (getline(openFile, line)) {
			valid_header_name_.insert(line);
		}
		openFile.close();
	}
}

RequestMessage::~RequestMessage() {}

const std::string &RequestMessage::GetMethod() const {
	return method_;
}

const std::string &RequestMessage::GetUri() const {
	return uri_;
}

const RequestMessage::header_map_type &RequestMessage::GetHeaderMap() const {
	return header_map_;
}

void RequestMessage::ParsingMessage(const std::string &message) {
	std::string buf;
	size_t index;

	index = message.find("R\n");
	buf = message.substr(0, index + 1);
	ParsingStartLine(buf);

	index = message.find("R\nR\n");
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

void IsValidHeaderName(std::string name, std::string value) {
	
}

void RequestMessage::SplitHeader(const std::string &header) {
	std::string name;
	std::string value;
	size_t index;

	index = header.find(": ");
	name = header.substr(0, index);
	value = header.substr(index + 2, header.length());
	header_map_[name] = value;
}

void RequestMessage::ParsingHeaders(const std::string headers) {
	size_t start = 0;
	size_t end;

	end = headers.find("R\n");
	size_t i = 0;
	while (end != std::string::npos) {
		SplitHeader(headers.substr(start, end - start));
		start = end + 2;
		end = headers.find("R\n", start);
	}
}
