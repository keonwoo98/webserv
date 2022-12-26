#include "response_message.hpp"
#include <string>
#include <sstream>

std::string ResponseMessage::ToString() {
	std::stringstream ss;

	ss << status_line_ << headers_ << body_;
	return ss.str();
}

ResponseMessage::ResponseMessage(int status_code, const std::string &reason_phrase)
	: total_length_(0), current_length_(0), status_line_(StatusLine(HttpVersion(), status_code, reason_phrase)) {
	headers_.AddServer();
}

void ResponseMessage::AppendBody(const std::string &body) {
	headers_.AddContentLength(body);
	body_ += body;
}

ResponseMessage::ResponseMessage() {

}

/** ResponseMessage 전체의 크기를 계산 */
void ResponseMessage::CalculateLength() {
	total_length_ = ToString().length();
}

bool ResponseMessage::HasMore() {
	if (current_length_ < total_length_) {
		return true;
	}
	return false;
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
