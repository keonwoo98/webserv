#ifndef CGI_PARSER_HPP
#define CGI_PARSER_HPP

#include "core_exception.hpp"
#include "response_message.hpp"

void AppendCgiHeader(ResponseMessage &response, const char *buf,
					 const int size);

void ParseCgiResult(ResponseMessage &response, const char *buf, const int size);

#endif
