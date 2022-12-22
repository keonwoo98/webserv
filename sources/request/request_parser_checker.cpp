#include <sstream>

#include "request_parser.hpp"
#include "server_info.hpp"

/*
inline bool isToken(char c);
inline bool isVChar(char c);
inline size_t hexstrToDec(std::string hex_string);

void CheckProtocol(RequestMessage & req_msg, const std::string & protocol);
bool CheckHeaderField(const RequestMessage & req_msg);
bool IsThereHost(const RequestMessage & req_msg);
bool CheckSingleHeaderName(const RequestMessage & req_msg);
bool RequestStartlineCheck(RequestMessage & req_msg, const ServerInfo & server_info);
bool RequestHeaderCheck(RequestMessage & req_msg, const ServerInfo & server_info);
*/

bool isToken(char c) {
	return (c == '!' || c == '#' || c ==  '$' || c ==  '%' || c ==  '&' \
	| c ==  '\'' || c ==  '*' || c ==  '+' || c ==  '-' || c ==  '.' \
	|| c ==  '^' || c ==  '_' || c ==  '`' || c ==  '|' || c ==  '~' \
	|| std::isalnum(c));
}

bool isVChar(char c)
{
	return (0x21 <= c && c <= 0x7E);
}

size_t hexstrToDec(std::string hex_string) {
	size_t n;

	std::istringstream(hex_string) >> std::hex >> n;
	return n;
}


void CheckProtocol(RequestMessage & req_msg, const std::string & protocol)
{
	size_t slash_pos = protocol.find('/');
	if (slash_pos == protocol.npos)	{
		req_msg.SetStatusCode(BAD_REQUEST);
		req_msg.SetConnection(false);
		return ;
	}
	std::string http = protocol.substr(0, slash_pos);
	std::string version = protocol.substr(slash_pos + 1);
	if (http != "HTTP") {
		req_msg.SetStatusCode(BAD_REQUEST);
		req_msg.SetConnection(false);
	}
	else if (version != "1.1") {
		req_msg.SetStatusCode(HTTP_VERSION_NOT_SUPPORTED);
		req_msg.SetConnection(false);
	}
	return;
}

bool CheckSingleHeaderName(const RequestMessage & req_msg)  {
	const std::string & target_headername = req_msg.GetTempHeaderName();
	if (target_headername.size() == 0)
		return false;
	if (req_msg.GetHeaders().find(target_headername) == req_msg.GetHeaders().end())
		return true;
	return false;
}

bool RequestStartlineCheck(RequestMessage & req_msg, const ServerInfo & server_info)
{
	// Method
	const std::vector<std::string> &allowed_methods = server_info.GetAllowMethods();
	const std::string &method = req_msg.GetMethod();
	if (find(allowed_methods.begin(), allowed_methods.end(), method) == allowed_methods.end())
	{
		req_msg.SetStatusCode(METHOD_NOT_ALLOWED);
		return (false);
	}
	else if ((method != "GET") && (method != "DELETE") && (method != "POST"))
	{
		req_msg.SetStatusCode(NOT_IMPLEMENTED);
		return (false);
	}

	// URI
	return (true);
}

bool RequestHeaderCheck(RequestMessage & req_msg, const ServerInfo & server_info)
{
	(void)server_info;
	if (req_msg.GetHeaders().find("host") == req_msg.GetHeaders().end()){
		req_msg.SetStatusCode(BAD_REQUEST);
		return (false);
	}

	return (true);
}
