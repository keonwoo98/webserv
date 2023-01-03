#include "response_message.hpp"
#include <string>
#include <sstream>

ResponseMessage::ResponseMessage()
	: total_length_(0), current_length_(0), status_line_(StatusLine()) {
	headers_.AddServer();
}

const std::string &ResponseMessage::GetBody() const {
	return body_;
}

int ResponseMessage::GetState() const {
	return state_;
}

void ResponseMessage::SetStatusLine(int status_code, const std::string &reason_phrase) {
	status_line_ = StatusLine(HttpVersion(), status_code, reason_phrase);
}

void ResponseMessage::SetState(int state) {
	state_ = state;
}

void ResponseMessage::SetContentLength() {
	headers_.AddContentLength(body_.length());
}

void ResponseMessage::EraseBody(size_t begin, size_t size) {
	body_.erase(begin, size);
}

ResponseMessage::ResponseMessage(int status_code, const std::string &reason_phrase)
	: total_length_(0), current_length_(0), status_line_(StatusLine(HttpVersion(), status_code, reason_phrase)) {
	headers_.AddServer();
}

void ResponseMessage::AppendBody(const char *body) {
	size_t body_length = strlen(body);
	headers_.AddContentLength(body_.length() + body_length);
	body_.append(body);
}

void ResponseMessage::AppendBody(const char *body, size_t count) {
	headers_.AddContentLength(body_.length() + count);
	body_.append(body, count);
}

void ResponseMessage::AddHeader(const std::string &key,
								const std::string &value) {
	headers_.Add(key, value);
}

void ResponseMessage::AddLocation(const std::string &uri) {
	headers_.AddLocation(uri);
}

void ResponseMessage::AddConnection(const std::string &connection) {
	headers_.AddConnection(connection);
}

bool ResponseMessage::IsDone() {
	if (current_length_ >= total_length_) {
		return true;
	}
	return false;
}

void ResponseMessage::AddCurrentLength(int send_len) {
	current_length_ += send_len;
}

void ResponseMessage::Clear() {
	status_line_.Clear();
	headers_.Clear();
	body_.clear();
	state_ = ResponseMessage::HEADER;
	total_length_ = 0;
	current_length_ = 0;
}

int ResponseMessage::BodySize() {
	return body_.length();
}

std::string ResponseMessage::GetErrorPagePath(ServerInfo server_info) {
	return server_info.GetErrorPagePath(status_line_.GetStatusCode());
}

bool ResponseMessage::IsErrorStatus() {
	if (status_line_.GetStatusCode() >= 400) {
		return true;
	}
	return false;
}

std::string ResponseMessage::ToString() {
	std::stringstream ss;

	std::string status_line = status_line_.ToString();
	std::string headers = headers_.ToString();
	total_length_ = status_line.length() + headers.length() + body_.length();

	ss << status_line << headers << body_;
	return ss.str();
}

std::ostream &operator<<(std::ostream &out, ResponseMessage message) {
	out << message.ToString();
	return out;
}
