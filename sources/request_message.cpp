#include "request_message.hpp"

#include <fstream>
#include <locale>
#include <sstream>

RequestMessage::RequestMessage()
	: content_size_(-1), is_chunked_(false), keep_alive_(true) {}

RequestMessage::~RequestMessage() {}

const std::string &RequestMessage::GetMethod() const { return method_; }

const std::string &RequestMessage::GetUri() const { return uri_; }

const RequestMessage::header_map_type &RequestMessage::GetHeaderMap() const {
	return header_map_;
}

void RequestMessage::SetMethod(const std::string &method) {
	CheckMethod(method);
	method_ = method;
}

void RequestMessage::SetUri(const std::string &uri) {
	CheckUri(uri);
	uri_ = uri;
}

void RequestMessage::SetHeader(
	const std::pair<std::string, std::string> &header) {
	CheckHeader(header);
	header_map_.insert(header);
}

void RequestMessage::SetBody(const std::string &body) { body_ = body; }

const std::string &RequestMessage::GetBody() const { return body_; }

void RequestMessage::CheckMethod(const std::string &method) const {
	// 구현 필요
	(void)method;
	return;
}

void RequestMessage::CheckUri(const std::string &uri) const {
	// 구현 필요
	(void)uri;
	return;
}

void RequestMessage::CheckHeader(
	const std::pair<std::string, std::string> &header) const {
	// 구현 필요
	(void)header;
	return;
}

bool RequestMessage::IsThereHost() const {
	if (header_map_.find("host") != header_map_.end()) {
		return true;
	}
	return false;
}

void RequestMessage::ParsingMessage(const std::string &message) {
	std::string buf;
	size_t index;

	index = message.find(CRLF);
	buf = message.substr(0, index + 1);
	ParsingStartLine(buf);

	index = message.find(DCRLF);
	buf = message.substr(buf.length(), index - buf.length() + 2);
	ParsingHeaders(buf);

	RequestChunkedMessage chunk_parser_functor_;
	buf = message.substr(index + 4, message.length() - (index + 4));
	body_ += chunk_parser_functor_(buf.c_str());
}

void RequestMessage::ParsingStartLine(const std::string start_line) {
	std::stringstream start_line_stream(start_line);
	std::string protocol;

	std::getline(start_line_stream, method_, SP);
	if (method_ != "GET" && method_ != "POST" && method_ != "DELETE")
		std::cerr << "error: " << method_ << std::endl;

	std::getline(start_line_stream, uri_, SP);

	std::getline(start_line_stream, protocol, CR);
	if (protocol != "HTTP/1.1") std::cerr << "error: " << protocol << std::endl;
}

void RequestMessage::SplitHeader(const std::string &header) {
	std::string name;
	std::string value;
	std::string::size_type colon_pos;

	colon_pos = header.find(":");
	name = header.substr(0, colon_pos);
	if (header[colon_pos + 1] != SP) {
		std::cerr << "error: <request header> No SP after colon" << method_
				  << std::endl;
		return;
	}
	value = header.substr(colon_pos + 2, header.length());
	header_map_[name] = value;
}

bool RequestMessage::CheckHeader(void) const { return true; }

void RequestMessage::ParsingHeaders(const std::string headers) {
	size_t start = 0;
	size_t end;

	end = headers.find(CRLF, 0);
	while (end != std::string::npos) {
		SplitHeader(headers.substr(start, end - start));
		start = end + 2;
		end = headers.find(CRLF, start);
	}
}

// bool RequestMessage::isTchar(char c)
// {
// 	return (std::isalnum(c) || c == '!' || c == '#' || c == '$' \
// 	|| c == '%' || c == '&' || c == '\'' || c == '|' || c == '~');
// }
