#include "request_parser.hpp"

#include <sstream>
#include <algorithm> // for std::transform

RequestParser::RequestParser() : parsing_state_(START_LINE) {}

RequestParser::~RequestParser() {}

int RequestParser::AppendMessage(const std::string &message) {
	message_.append(message);
	switch (parsing_state_) {
		case START_LINE:
			ParsingStartLine(message);
			break;
		case HEADERS:
			ParsingHeader(message);
			break;
		case BODY:
			ParsingBody(message);
			break;
		case DONE:
			break;
	}
	return message.length();
}

bool RequestParser::IsDone() const { return parsing_state_ == DONE; }

void RequestParser::ResetState() {
	parsing_state_ = START_LINE;
	message_.clear();
}

void RequestParser::ParsingStartLine(const std::string &start_line) {
	std::stringstream ss(start_line);
	std::string buf;

	// change parsing state
	parsing_state_ = HEADERS;

	// set requeset method
	std::getline(ss, buf, ' ');
	request_.SetMethod(buf);

	// set request uri
	std::getline(ss, buf, ' ');
	request_.SetUri(buf);

	// set http version
	std::getline(ss, buf, '\r');
	request_.SetHttpVersion(buf);
}

void RequestParser::ParsingHeader(const std::string &header) {
	std::string name;
	std::string value;
	size_t index;
	// size_t len;

	if (header == "\r\n") {
		parsing_state_ = BODY;
		return;
	}

	index = header.find(":");
	// if (index == std::string::npos ||
	// 	header.find(":", index) != std::string::npos) {
	// 	perror("400 Bad Request");
	// }

	// len = header.length();
	// if (header[len - 2] != '\r' || header[len - 1] != '\n') {
	// 	perror("400 Bad Requeset");
	// }

	name = header.substr(0, index);
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	value = header.substr(index + 2, header.length());
	request_.SetHeader(std::pair<std::string, std::string>(name, value));
}

void RequestParser::ParsingBody(const std::string &body) {
	// chunked 이면 chunked message에 맞게 파싱 후 set 해야함.
	// unchunked 이면 길이 만큼 읽고  set 해야함.
	// setbody가 호출되고 나면 Parsing_state를 done으로 바꾸기 때문.
	// 아마 append body 추가해야 할 듯.
	if (false) { //unchunked
		request_.SetBody(body);
		parsing_state_ = DONE;
	} else {// chunked
		request_.SetBody(chunk_parser_(body.c_str()));
		if (chunk_parser_.IsChunkedDone() == true)
		{
			chunk_parser_.ResetChunkState();
			parsing_state_ = DONE;
		}
	}
	// 예외 처리는 나중에
}

std::ostream &operator<<(std::ostream &os, const RequestParser &parser) {
	os << parser.message_;
	return os;
}
