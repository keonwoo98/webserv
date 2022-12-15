#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <iostream>

#include "request_message.hpp"
#include "request_chunked_message.hpp"

class RequestParser {
   public:
	enum State { START_LINE, HEADERS, BODY, DONE };

	RequestParser(const RequestMessage &request);
	~RequestParser();

	int AppendMessage(const std::string &message);
	bool IsDone() const;
	void ResetState();

	friend std::ostream &operator<<(std::ostream &os,
									const RequestParser &parser);

   private:
	std::string message_;
	std::string buf_;
	size_t pos_;

	State state_;

	RequestMessage request_;
	RequestChunkedParser chunk_parser_;

	void ParsingMessage();
	void ParsingStartLine();
	void ParsingHeader();
	void ParsingBody();

	bool FillBuffer();
	void MovePosition();
};

#endif
