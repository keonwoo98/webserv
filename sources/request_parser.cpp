#include "request_parser.hpp"

#include <algorithm>  // for std::transform
#include <sstream>

RequestParser::RequestParser() : start_(0), end_(0), state_(START_LINE) {}

RequestParser::~RequestParser() {}

int RequestParser::AppendMessage(const std::string &message) {
	message_.append(message);
	while (state_ != DONE && end_ < message_.length()) {
		switch (state_) {
			case START_LINE:
				ParsingStartLine();
				break;
			case HEADERS:
				ParsingHeader();
				break;
			case BODY:
				ParsingBody();
				break;
			case DONE:
				break;
		}
	}
	return message.length();
}

bool RequestParser::IsDone() const { return state_ == DONE; }

void RequestParser::ResetState() {
	state_ = START_LINE;
	message_.clear();
}

void RequestParser::ParsingStartLine() {
	if (ChangeEndPosition() == false) {
		return;
	}
	std::string start_line = message_.substr(start_, end_ - start_);
	std::stringstream ss(start_line);
	std::string buf;

	// set requeset method
	std::getline(ss, buf, ' ');
	request_.SetMethod(buf);

	// set request uri
	std::getline(ss, buf, ' ');
	request_.SetUri(buf);

	// set http version
	std::getline(ss, buf, '\r');
	request_.SetHttpVersion(buf);

	// change parsing state
	state_ = HEADERS;
	ChangeStartPosition();
}

void RequestParser::ParsingHeader() {
	if (ChangeEndPosition() == false) {
		return;
	}
	std::string header = message_.substr(start_, end_ - start_);
	std::string name;
	std::string value;
	size_t index;

	if (header == "\r\n") {
		state_ = BODY;
		ChangeStartPosition();
		return;
	}

	index = header.find(":");
	name = header.substr(0, index);
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	value = header.substr(index + 2, header.length());
	request_.SetHeader(std::pair<std::string, std::string>(name, value));

	ChangeStartPosition();
}

void RequestParser::ParsingBody() {
	// chunked 이면 chunked message에 맞게 파싱 후 set 해야함.
	// unchunked 이면 길이 만큼 읽고  set 해야함.
	// setbody가 호출되고 나면 Parsing_state를 done으로 바꾸기 때문.
	// 아마 append body 추가해야 할 듯.
	ChangeEndPosition();
	std::string body = message_.substr(start_, end_ - start_);
	if (false) {  // unchunked
		request_.SetBody(body);
		state_ = DONE;
	} else {  // chunked
		request_.SetBody(chunk_parser_(body.c_str()));
		if (chunk_parser_.IsChunkedDone() == true) {
			state_ = DONE;
		}
	}
	ChangeStartPosition();
	// 예외 처리는 나중에
}

void RequestParser::ChangeStartPosition() { start_ = end_; }

bool RequestParser::ChangeEndPosition() {
	if (state_ == START_LINE || state_ == HEADERS) {
		end_ = message_.find("\r\n", start_);
		if (end_ == std::string::npos) {
			return false;
		}
		end_ += 2;
	} else if (state_ == BODY) {
		end_ = message_.length();
	}
	return true;
}

std::ostream &operator<<(std::ostream &os, const RequestParser &parser) {
	os << parser.message_;
	return os;
}
