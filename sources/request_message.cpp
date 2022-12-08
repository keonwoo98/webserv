#include "request_message.hpp"

#include <fstream>
#include <sstream>

RequestMessage::RequestMessage() {}

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

void RequestMessage::SetHeader(const std::pair<std::string, std::string> &header) {
	CheckHeader(header);
	header_map_.insert(header);
}

void RequestMessage::SetBody(const std::string &body) {
	body_ = body;
}

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

void RequestMessage::CheckHeader(const std::pair<std::string, std::string> &header) const {
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
