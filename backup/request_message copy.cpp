#include "request_message.hpp"

#include <fstream>
#include <sstream>

RequestMessage::RequestMessage() : state_(RequestMessage::START_LINE) {
	header_map_["host"] = "";
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

int RequestMessage::AppendMessage(const std::string &message) {
	message_.append(message);

	if (state_ == START_LINE) {
		ParsingStartLine(message);
		state_ = HEADERS;
	} else if (state_ == HEADERS && message == "\r\n") {
		std::string header;

		header = message_.substr()
		ParsingHeaders();
	}
}

void RequestMessage::ParsingMessage() {
	std::string buf;
	size_t index;
	size_t start;
	size_t end;

	start = message_.find("\r\n") + 2;
	end = message_.find("\r\n\r\n");
	ParsingHeaders(message_.substr(start, end - start));

	body_ = message_.substr(index + 4, message_.length() - (index + 4));
}

void RequestMessage::ParsingStartLine(const std::string start_line) {
	std::stringstream ss(start_line);
	std::string buf;

	std::getline(ss, method_, ' ');
	if (method_ != "GET" && method_ != "POST" && method_ != "DELETE")
		std::cerr << "error: " << method_ << std::endl;

	std::getline(ss, uri_, ' ');

	std::getline(ss, buf, '\r');
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

bool RequestMessage::ValidateTransfereEncoding(
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

bool RequestMessage::ValidateHeader(
	const RequestMessage::header_map_type::iterator &header) const {
	if (header->first == "transfer-encoding") {
		ValidateTransfereEncoding(header->second);
		return true;
	}
	return false;
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

	end = headers.find("\r\n");
	while (end != std::string::npos) {
		SplitHeader(headers.substr(start, end - start));
		start = end + 2;
		end = headers.find("\r\n", start);
	}
}

std::ostream &operator<<(std::ostream &os, const RequestMessage &request) {
	os << request.method_ << " " << request.uri_ << " HTTP/1.1\r\n";
	for (RequestMessage::header_map_type::const_iterator it =
			 request.header_map_.begin();
		 it != request.header_map_.end(); ++it) {
			os << it->first << ": " << it->second << "\r\n";
	}
	os << "\r\n";
	os << "body" << std::endl;
	os << request.body_;
	return os;
}

void RequestMessage::ClearMessage() {
	method_.clear();
	uri_.clear();
	header_map_["host"] = "";
	header_map_["transfer-encoding"] = "";
	header_map_["connection"] = "";
	header_map_["content-range"] = "";
	body_.clear();
}
