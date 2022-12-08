#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <iostream>

#include "request_message.hpp"

class RequestParser {
   public:
	enum ParsingState { START_LINE, HEADERS, BODY, DONE };

	RequestParser();
	~RequestParser();

	int AppendMessage(const std::string &message);
	bool IsDone() const;
	void ResetState();

	friend std::ostream &operator<<(std::ostream &os,
									const RequestParser &parser);

   private:
	std::string message_;
	ParsingState parsing_state_;
	RequestMessage request_;

	void ParsingStartLine(const std::string &start_line);
	void ParsingHeader(const std::string &header);
	void ParsingBody(const std::string &body);
};

#endif

/*

void RequestMessage::ParsingMessage(const std::string &message) {
	std::string buf;
	size_t index;

	index = message.find(CRLF);
	buf = message.substr(0, index + 1);
	ParsingStartLine(buf);

	index = message.find(DCRLF);
	buf = message.substr(buf.length(), index - buf.length() + 2);
	ParsingHeaders(buf);

	RequestChunkedMessage chunk_parser_functor_;
	buf = message.substr(index + 4, message.length() - (index + 4));
	body_ += chunk_parser_functor_(buf.c_str());
}

void RequestMessage::ParsingStartLine(const std::string start_line) {
	std::stringstream start_line_stream(start_line);
	std::string protocol;

	std::getline(start_line_stream, method_, SP);
	if (method_ != "GET" && method_ != "POST" && method_ != "DELETE")
		std::cerr << "error: " << method_ << std::endl;

	std::getline(start_line_stream, uri_, SP);

	std::getline(start_line_stream, protocol, CR);
	if (protocol != "HTTP/1.1") std::cerr << "error: " << protocol << std::endl;
}

void RequestMessage::SplitHeader(const std::string &header) {
	std::string name;
	std::string value;
	std::string::size_type colon_pos;

	colon_pos = header.find(":");
	name = header.substr(0, colon_pos);
	if (header[colon_pos + 1] != SP) {
		std::cerr << "error: <request header> No SP after colon" << method_
				  << std::endl;
		return;
	}
	value = header.substr(colon_pos + 2, header.length());
	header_map_[name] = value;
}

bool RequestMessage::CheckHeader(void) const { return true; }

void RequestMessage::ParsingHeaders(const std::string headers) {
	size_t start = 0;
	size_t end;

	end = headers.find(CRLF, 0);
	while (end != std::string::npos) {
		SplitHeader(headers.substr(start, end - start));
		start = end + 2;
		end = headers.find(CRLF, start);
	}
}
*/
