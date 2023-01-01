#include "request_message.hpp"

#include <fstream>
#include <locale>
#include <sstream>

RequestMessage::RequestMessage(int client_max_body_size)
  : client_max_body_size_(client_max_body_size) {
	this->Clear();
}

RequestMessage::~RequestMessage() {}

void RequestMessage::Clear() {
	content_size_ = 0;
	is_chunked_ = false;
	keep_alive_ = true;

	state_ = START_METHOD;
	temp_header_name_.clear();
	temp_header_value_.clear();

	last_chunk_flag_ = false;
	chunk_size_ = 0;
	chunk_size_str_.clear();
	chunk_body_.clear();
	error_msg_.clear();

	method_.clear();
	uri_.clear();
	http_version_.clear();
	headers_.clear();
	body_.clear();
}

void RequestMessage::AppendMethod(char c) {
	method_ += c;
}

void RequestMessage::AppendUri(char c) {
	uri_ += c;
}

void RequestMessage::AppendProtocol(char c) {
	http_version_ += c;
}

size_t RequestMessage::AppendBody(const std::string & str) { 
	body_ += str;
	return (str.size());
}

void RequestMessage::AppendChunkSizeStr(char c) {
	chunk_size_str_ += c;
}

void RequestMessage::AppendChunkBody(char c) {
	chunk_body_ += c;
}

void RequestMessage::AppendHeaderName(char c) {
	temp_header_name_ += c;
}

void RequestMessage::AppendHeaderValue(char c) {
	temp_header_value_ += c;
}

void RequestMessage::AddHeaderField() {
	const std::string &key = temp_header_name_;
	const std::string &value = temp_header_value_;
	// TODO : 앞 뒤 whitespace 뺄 수 있으면 좋을 듯.
	headers_.insert(std::make_pair(key, value));
	temp_header_name_.clear();
	temp_header_value_.clear();
}

std::ostream &operator<<(std::ostream &os, const RequestMessage &req_msg) {
	os << "======[    Request Message     ]========" << std::endl;
	os << "Method: " <<  req_msg.GetMethod() << std::endl;
	os << "Target: " << req_msg.GetUri() << std::endl;
	os << "Headers: " << std::endl;
	RequestMessage::headers_type::const_iterator it;
	for (it = req_msg.GetHeaders().begin(); it !=  req_msg.GetHeaders().end() ; it++)
		os << "  " << it->first << ": " << it->second << std::endl;
	os << "Body: " << std::endl;
	os << req_msg.GetBody() << std::endl;
	os << "---------------------------------------" << std::endl;
	os << "[Body Size ] : " << req_msg.GetContentSize() << std::endl;
	os << "[Connection] : " << (req_msg.ShouldClose() ? "close" : "alive") << std::endl;
	os << "[   URI    ] : " ;
	const std::string &uri = req_msg.GetResolvedUri();
	os << uri;
	os << std::endl;
	os << "=======================================" << std::endl;
	return os;
}
