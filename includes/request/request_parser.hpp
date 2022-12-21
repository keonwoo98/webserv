#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <iostream>

#include "server_info.hpp"
#include "request_message.hpp"
#include "request_chunked_message.hpp"

class RequestParser {
   public:
	RequestParser();
	~RequestParser();

	void operator() (RequestMessage & req_msg, const char * input);
	void operator() (RequestMessage & req_msg, const ServerInfo & server_info);

   private:

	/* Parser */
	RequestChunkedParser chunked_parser_;
	void ParseStartLine(RequestMessage & req_msg, char c);
	void ParseHeader(RequestMessage & req_msg, char c);
	size_t ParseBody(RequestMessage & req_msg, const char *c);
	size_t ParseUnchunkedBody(RequestMessage & req_msg, const char *c);
	size_t ParseChunkedBody(RequestMessage & req_msg, const char *c);

	/* Checker */
	void CheckProtocol(RequestMessage & req_msg, const std::string & protocol);

	bool CheckHeaderField(const RequestMessage & req_msg);
	bool IsThereHost(const RequestMessage & req_msg) const;
	bool CheckSingleHeaderName(const RequestMessage & req_msg) const;
};

bool RequestStartlineCheck(RequestMessage & req_msg, const ServerInfo & server_info);
bool RequestHeaderCheck(RequestMessage & req_msg, const ServerInfo & server_info);
#endif
