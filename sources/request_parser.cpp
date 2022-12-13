#include "request_parser.hpp"

#include <algorithm>  // for std::transform
#include <sstream>

RequestParser::RequestParser() : pos_(0), state_(START_LINE) {}

RequestParser::~RequestParser() {}

int RequestParser::AppendMessage(const std::string &message) {
	message_.append(message);
	ParsingMessage();
	return message.length();
}

void RequestParser::ParsingMessage() {
	while (state_ != DONE && pos_ < message_.length()) {
		if (FillBuffer() == true) {
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
			MovePosition();
		}
	}
}

bool RequestParser::IsDone() const { return state_ == DONE; }

void RequestParser::ResetState() {
	state_ = START_LINE;
	message_.clear();
}

void RequestParser::ParsingStartLine() {
	std::stringstream ss(buf_);
	std::string tmp;

	// set requeset method
	std::getline(ss, tmp, ' ');
	request_.SetMethod(tmp);

	// set request uri
	std::getline(ss, tmp, ' ');
	request_.SetUri(tmp);

	// set http version
	std::getline(ss, tmp, '\r');
	request_.SetHttpVersion(tmp);

	// change parsing state
	state_ = HEADERS;
}

void RequestParser::ParsingHeader() {
	std::string name;
	std::string value;
	size_t colon;

	if (buf_ == "\r\n") {
		// host 있는지 확인.
		if (this->request_.GetHeaderMap().find("host") == this->request_.GetHeaderMap().end())
			throw std::runtime_error("No Host");
		if (this->request_.GetMethod() == "POST")
			state_ = BODY;
		else
			state_ = DONE;
	} else {
		colon = buf_.find(":");
		name = buf_.substr(0, colon);
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		value = buf_.substr(colon + 1, buf_.length() - (colon + 1));
		std::cout << name << ":" << value;
		request_.SetHeader(std::pair<std::string, std::string>(name, value));
	}
}

void RequestParser::ParsingBody() {
	// chunked 이면 chunked message에 맞게 파싱 후 set 해야함.
	// unchunked 이면 길이 만큼 읽고  set 해야함.
	// setbody가 호출되고 나면 Parsing_state를 done으로 바꾸기 때문.
	// 아마 append body 추가해야 할 듯.
	if (request_.IsChunked() == false) {  // unchunked
		request_.SetBody(buf_);
		state_ = DONE;
	} else {  // chunked
		request_.AppendBody(chunk_parser_(buf_.c_str()));
		if (chunk_parser_.IsChunkedDone() == true) {
			state_ = DONE;
		}
	}
	// 예외 처리는 나중에
}

bool RequestParser::FillBuffer() {
	size_t end;
	if (state_ == START_LINE || state_ == HEADERS) {
		end = message_.find("\r\n", pos_);
		if (end == std::string::npos) {
			return false;
		}
		end += 2;
	} else {
		end = message_.length();
	}
	buf_ = message_.substr(pos_, end - pos_);
	return true;
}

void RequestParser::MovePosition() { pos_ += buf_.length(); }

std::ostream &operator<<(std::ostream &os, const RequestParser &parser) {
	os << parser.message_;
	return os;
}
