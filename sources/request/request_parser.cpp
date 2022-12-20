#include <algorithm>  // for std::transform
#include <sstream>

#include "character_color.hpp"
#include "request_parser.hpp"

RequestParser::RequestParser(RequestMessage &request) : pos_(0), state_(START_LINE), request_(request) {}

RequestParser::~RequestParser() {}

int RequestParser::AppendMessage(const std::string &message) {
	message_.append(message);
	ParsingMessage();
	return message.length();
}

void RequestParser::ParsingMessage() {
	while (state_ != DONE && FillBuffer() == true) {
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

bool RequestParser::IsDone() const { return state_ == DONE; }

void RequestParser::Reset() {
	state_ = START_LINE;
	pos_ = 0;
	message_.clear();
	buf_.clear();
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
		value = value.substr(0, value.length() - 2);
		request_.SetHeader(std::pair<std::string, std::string>(name, value));
	}
}

void RequestParser::ParsingBody() {
	if (request_.IsChunked() == false) {  // unchunked
		int size_left = request_.GetContentSize() - request_.GetBody().size();
		if (size_left <= (int)buf_.size()) {
			request_.AppendBody(buf_.substr(0, size_left));
			state_ = DONE;
		} else {
			request_.AppendBody(buf_);
		}
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
	if (state_ == START_LINE || state_ == HEADERS ||
		(state_ == BODY && request_.IsChunked())) {
		end = message_.find("\r\n", pos_);
		if (end == std::string::npos) {
			return false;
		}
		end += 2;
	} else {
		end = message_.length();
	}
	buf_ = message_.substr(pos_, end - pos_);
	return (!buf_.empty());
}

void RequestParser::MovePosition() { pos_ += buf_.length(); }

std::ostream &operator<<(std::ostream &os, const RequestParser &parser) {
	os << C_ITALIC << C_LIGHTCYAN << "======[Printing Request input]========" << C_RESET << C_FAINT << C_CYAN << std::endl;
	os << C_UNDERLINE << "Method" << C_RESET << C_FAINT << C_CYAN <<  ": " << parser.request_.GetMethod() << std::endl;
	os << C_UNDERLINE << "Target" << C_RESET << C_FAINT << C_CYAN <<  ": " << parser.request_.GetUri() << std::endl;
	os << C_UNDERLINE << "Heades" << C_RESET << C_FAINT << C_CYAN <<  ": " << std::endl;
	RequestMessage::header_map_type::const_iterator it;
	for (it = parser.request_.GetHeaderMap().begin(); it !=  parser.request_.GetHeaderMap().end() ; it++)
		os << "  " << it->first << ": " << it->second << std::endl;
	os << C_UNDERLINE << "Body" << C_RESET << C_FAINT << C_CYAN <<  ": " << std::endl;
	os << parser.request_.GetBody() << C_RESET << std::endl;
	os << C_ITALIC << C_LIGHTCYAN << "=======================================" << C_RESET << std::endl;
	return os;
}
