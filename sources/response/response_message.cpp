#include "response_message.hpp"
#include <string>
#include <sstream>

/**
 * ToString : Response Message를 문자열로 바꿔주는 메서드
 * total_length_ 계산
 * @return Respone Message(std::string)
 */
std::string ResponseMessage::ToString() {
	std::stringstream ss;

	std::string status_line = status_line_.ToString();
	std::string headers = headers_.ToString();
	total_length_ = status_line.length() + headers.length() + body_.length();

	ss << status_line << headers << body_;
	return ss.str();
}

ResponseMessage::ResponseMessage(int status_code, const std::string &reason_phrase)
	: total_length_(0), current_length_(0), status_line_(StatusLine(HttpVersion(), status_code, reason_phrase)) {
	headers_.AddServer();
}

void ResponseMessage::AppendBody(const std::string &body) {
	headers_.AddContentLength(body_.length() + body.length());
	body_.append(body);
}

void ResponseMessage::AppendBody(const std::string &body, size_t count) {
	headers_.AddContentLength(body_.length() + count);
	body_.append(body, count);
}

ResponseMessage::ResponseMessage() {

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
	total_length_ = 0;
	current_length_ = 0;
}

std::ostream &operator<<(std::ostream &out, ResponseMessage message) {
	out << message.ToString();
	return out;
}
