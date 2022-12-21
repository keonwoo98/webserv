#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <iostream>

#include "request_message.hpp"
#include "request_chunked_message.hpp"

class RequestParser {
   public:
	RequestParser();
	~RequestParser();

	void operator() (RequestMessage & req_msg, const char * input);

   private:

	RequestChunkedParser chunked_parser_;
	void ParseStartLine(RequestMessage & req_msg, char c);
	void ParseHeader(RequestMessage & req_msg, char c);
	size_t ParseBody(RequestMessage & req_msg, const char *c);
	size_t ParseUnchunkedBody(RequestMessage & req_msg, const char *c);
	size_t ParseChunkedBody(RequestMessage & req_msg, const char *c);
};

#endif
