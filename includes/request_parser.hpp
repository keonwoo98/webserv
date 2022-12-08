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
