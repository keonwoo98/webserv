#include "request_message.hpp"

#include <fstream>
#include <locale>
#include <sstream>

RequestMessage::RequestMessage()
	: content_size_(-1), is_chunked_(false), keep_alive_(true) {}

RequestMessage::~RequestMessage() {}

const std::string &RequestMessage::GetMethod() const { return method_; }

const std::string &RequestMessage::GetUri() const { return uri_; }

const std::string &RequestMessage::GetHttpVersion() const {
	return http_version_;
}

const RequestMessage::header_map_type &RequestMessage::GetHeaderMap() const {
	return header_map_;
}

void RequestMessage::SetMethod(const std::string &method) {
	// if method is invalid, it throw HttpException
	CheckMethod(method);
	method_ = method;
}

void RequestMessage::SetUri(const std::string &uri) {
	// if uri is invalid, it throw HttpException
	CheckUri(uri);
	uri_ = uri;
}

void RequestMessage::SetHttpVersion(const std::string &http_version) {
	CheckHttpVersion(http_version);
	http_version_ = http_version;
}

void RequestMessage::SetHeader(
	const std::pair<std::string, std::string> &header) {
	CheckHeader(header);

	std::string key = header.first;
	std::string value = header.second;

	if (key == "connection") {
		if (value.find("close")) {
			keep_alive_ = false;
		}
	} else if (key == "content-length") {
		if (method_ != "POST") {
			content_size_ = 0;
			// is_chunked_ = false;
		} else {
			content_size_ = atoi(value.c_str());
			// is_chunked_ = true;
		}
	} else if (key == "transfer-encoding" && method_ == "POST") {
		is_chunked_ = true;
	}
	header_map_.insert(header);
}

void RequestMessage::SetBody(const std::string &body)
{
	body_ = body;
}

void RequestMessage::AppendBody(const std::string &body)
{
	body_ += body;
}

const std::string &RequestMessage::GetBody() const { return body_; }

void RequestMessage::CheckMethod(const std::string &method) const {
	if (method != "GET" && method != "POST" && method != "DELETE") {
		throw HttpException(400);
	}
}

void RequestMessage::CheckUri(const std::string &uri) const {
	if (uri[0] != '/') {
		throw HttpException(400);
	}
	return;
}

void RequestMessage::CheckHttpVersion(const std::string &http_version) const {
	if (http_version != "HTTP/1.1") {
		throw HttpException(400);
	}
}

void RequestMessage::CheckHeader(
	const std::pair<std::string, std::string> &header) const {
	std::string key = header.first;
	std::string value = header.second;

	if (header_map_.find(key) != header_map_.end()) {
		throw HttpException(400);
	}

	if (key == "content-length") {
		for (size_t i = 0; i < value.size(); ++i) {
			if (!isdigit(value[i]) && !isspace(value[i])) {
				throw HttpException(400);
			}
		}
	} else if (key == "host") {
		// if value is not only one, throw error
		if (CountValue(header.second) != 1) {
			throw HttpException(400);
		}
	} else if (key == "transfer-encoding") {
		size_t pos = value.find("chunked");
		if (pos == std::string::npos) {
			throw HttpException(501);
		}
	}
}

size_t RequestMessage::CountValue(std::string value) const {
	size_t cnt = 0;
	value.insert(0, " ");
	for (size_t i = 1; i < value.size(); ++i) {
		if (isspace(value[i - 1]) && !isspace(value[i])) {
			++cnt;
		}
	}
	return cnt;
}

bool RequestMessage::IsThereHost() const {
	if (header_map_.find("host") != header_map_.end()) {
		return true;
	}
	return false;
}

bool RequestMessage::IsChunked() const {
	return this->is_chunked_;
}

