#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <iostream>

#include "request_message.hpp"

class RequestParser {
   public:
	enum State { START_LINE, HEADERS, BODY, DONE };

	RequestParser();
	~RequestParser();

	int AppendMessage(const std::string &message);
	bool IsDone() const;
	void ResetState();

	friend std::ostream &operator<<(std::ostream &os,
									const RequestParser &parser);

   private:
	std::string message_;
	size_t start_;
	size_t end_;

	State state_;

	RequestMessage request_;
	RequestMessage::RequestChunkedMessage chunk_parser_;

	void ParsingStartLine();
	void ParsingHeader();
	void ParsingBody();

	void ChangeStartPosition();
	bool ChangeEndPosition();
};

#endif
