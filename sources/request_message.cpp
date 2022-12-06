#include "request_message.hpp"

#include <fstream>
#include <sstream>

RequestMessage::RequestMessage() {
	header_map_["host"] = "";
	header_map_["keep-alive"] = "";
	header_map_["transfer-encoding"] = "";
	header_map_["connection"] = "";
	header_map_["content-range"] = "";
}

RequestMessage::~RequestMessage() {}

const std::string &RequestMessage::GetMethod() const { return method_; }

const std::string &RequestMessage::GetUri() const { return uri_; }

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
	buf = message.substr(buf.length(), index - buf.length() + 2);
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
	if (buf != "HTTP/1.1") std::cerr << "error: " << buf << std::endl;
}

bool RequestMessage::IsInHeaderMap(const std::string &name) const {
	std::string lower_name = name;
	for (size_t i = 0; i < name.size(); ++i) {
		tolower(lower_name[i]);
	}
	if (header_map_.find(lower_name) != header_map_.end()) {
		return true;
	}
	return false;
}

bool RequestMessage::IsThereHostInHeaders() const {
	if (header_map_.at("host") != "") {
		return true;
	}
	return false;
}

bool RequestMessage::TransferEncodingValidation(
	const std::string &value) const {
	size_t start = 0;
	size_t end = value.find(",");
	std::string buf;

	while (end != std::string::npos) {
		buf = value.substr(start, end - start);
		if (method_ == "GET" && buf != "chunked") {
			perror("501 Not Implemented");
			return false;
		} else {
			perror("405 Not Allowed");	// 찾아보기
			return false;
		}
	}
	return true;
}

bool RequestMessage::HeaderValidation(
	const RequestMessage::header_map_type::iterator &header) const {
	if (header->first == "transfer-encoding") {
		TransferEncodingValidation(header->second);
		return true;
	}
}

void RequestMessage::SplitHeader(const std::string &header) {
	std::string name;
	std::string value;
	size_t index;
	size_t len;

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
	if (IsInHeaderMap(name) == true) {
		value = header.substr(index + 2, header.length());
		header_map_[name] = value;
	}
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
