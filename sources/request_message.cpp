#include "request_message.hpp"

#include <fstream>
#include <locale>
#include <sstream>

RequestMessage::RequestMessage()
	: content_size_(-1), is_chunked_(false), keep_alive_(true) {}

RequestMessage::~RequestMessage() {}

const std::string &RequestMessage::GetMethod() const { return method_; }

const std::string &RequestMessage::GetUri() const { return uri_; }

const std::string &RequestMessage::GetHttpVersion() const { return http_version_; }

const RequestMessage::header_map_type &RequestMessage::GetHeaderMap() const {
	return header_map_;
}

void RequestMessage::SetMethod(const std::string &method) {
	try {
		CheckMethod(method);
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	method_ = method;
}

void RequestMessage::SetUri(const std::string &uri) {
	try {
		CheckUri(uri);
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
	uri_ = uri;
}

void RequestMessage::SetHttpVersion(const std::string &http_version) {
	try {
		CheckHttpVersion(http_version);
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	http_version_ = http_version;
}

void RequestMessage::SetHeader(
	const std::pair<std::string, std::string> &header) {
	try {
		CheckHeader(header);
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	header_map_.insert(header);
}

void RequestMessage::SetBody(const std::string &body) { body_ = body; }

const std::string &RequestMessage::GetBody() const { return body_; }

void RequestMessage::CheckMethod(const std::string &method) const {
	if (method != "GET" && method != "POST" && method != "DELETE") {
		throw HttpException::E400();
	}
}

void RequestMessage::CheckUri(const std::string &uri) const {
	// 구현 필요
	(void)uri;
	return;
}

void RequestMessage::CheckHttpVersion(const std::string &http_version) const {
	if (http_version != "HTTP/1.1") {
		throw HttpException::E400();
	}
}

void RequestMessage::CheckHeader(
	const std::pair<std::string, std::string> &header) const {
	if (header.first.find(" ") != std::string::npos ||
		header.second.find(":") != std::string::npos) {
		throw HttpException::E400();
	}
}

bool RequestMessage::IsThereHost() const {
	if (header_map_.find("host") != header_map_.end()) {
		return true;
	}
	return false;
}
