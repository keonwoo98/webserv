#include <sstream>
#include <algorithm>

#include "request_parser.hpp"
#include "status_code.hpp"
#include "server_info.hpp"
#include "location_info.hpp"

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

static bool IsThereHost(const RequestMessage &req_msg);
static bool CheckMethod(RequestMessage &req_msg, const std::vector<std::string> & allowed);
static bool CheckBodySize(RequestMessage &req_msg);
// StartLine에서 프로토콜 다 받자마자 확인
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

// 각 headername이 중복인지 확인
bool CheckSingleHeaderName(const RequestMessage & req_msg)  {
	const std::string & target_headername = req_msg.GetTempHeaderName();
	if (target_headername.size() == 0)
		return false;
	if (req_msg.GetHeaders().find(target_headername) == req_msg.GetHeaders().end())
		return true;
	return false;
}


// TODO : RequestMessage::GetServerName만들기 
// Header를 다 받으면 StartLine, HeaderField 전체적으로 검사
void CheckRequest(RequestMessage &req_msg, const std::vector<ServerInfo> &server_infos)
{
	if (IsThereHost(req_msg) == false) {
		req_msg.SetConnection(false);
		req_msg.SetStatusCode(BAD_REQUEST);
	} else {
		(void)server_infos;
		/*
			ServerInfo Select하는 부분
			ClientSocket::FindServerInfoWithHost(const std::string &host) { (void)host; }
			ClientSocket::FindLocationWithUri(const std::string &uri) { (void)uri; }
		*/
		// GetAllowedMethod();
		// GetClientMaxBodySize();
		std::vector<std::string> allowed; // temp
		req_msg.SetClientMaxBodySize(42); // temp

		if (CheckMethod(req_msg, allowed) == false) {
			return ;
		} else if (CheckBodySize(req_msg) == false) {
			return ;
		} else if (req_msg.GetMethod() != "POST") {
			req_msg.SetState(DONE);
		} else if (req_msg.IsChunked() == true) {
			req_msg.SetState(BODY_CHUNK_START);
		} else {
			req_msg.SetState(BODY_NONCHUNK);
		}
	}
}

bool IsThereHost(const RequestMessage &req_msg) {
	const RequestMessage::headers_type &headers_map = req_msg.GetHeaders();
	RequestMessage::headers_type::const_iterator it = headers_map.find("host");
	RequestMessage::headers_type::const_iterator it2 = headers_map.end();
	if (it == it2) {
		return (false);
	}
	return (true);
}

bool CheckMethod(RequestMessage &req_msg, const std::vector<std::string> & allowed) {
	const std::string &method = req_msg.GetMethod();
	if ((allowed.size()) && (std::find(allowed.begin(), allowed.end(), method) == allowed.end())) {
		req_msg.SetStatusCode(METHOD_NOT_ALLOWED);
		return (false);
	}
	if ((method != "GET") && (method != "POST") && (method != "POST")){
		req_msg.SetStatusCode(NOT_IMPLEMENTED);
		return (false);
	}
	
	const RequestMessage::headers_type & headers = req_msg.GetHeaders();
	RequestMessage::headers_type::const_iterator key;
	key = headers.find("transfer-encoding"); 
	if (key != headers.end()) {
		if (key->second != "chunked") {
			req_msg.SetStatusCode(BAD_REQUEST);
			return (false);
		}
		req_msg.SetChunked(true);
	}
	key = headers.find("content-length"); 
	if (key != headers.end()) {
		req_msg.SetContentSize(atoi(key->second.c_str()));
	}
	key = headers.find("connection"); 
	if ((key != headers.end()) && (key->second == "close")) {
		req_msg.SetConnection(false);
	}
	return (true);
}

/*
 * client body size 체크
 * POST일 때, 
*/
bool CheckBodySize(RequestMessage &req_msg) {
	const RequestMessage::headers_type &headers_map = req_msg.GetHeaders();
	RequestMessage::headers_type::const_iterator it = headers_map.find("content-length");

	if ( (it == headers_map.end()) && (req_msg.IsChunked() == false) && req_msg.GetMethod() == "POST")
	{
		req_msg.SetStatusCode(LENGTH_REQUIRED);
		return (false);
	}
	else if ( (it != headers_map.end()) && (req_msg.IsChunked() == true) ) {
		req_msg.SetConnection(false);
		return (true);
	}
	else if ( (it != headers_map.end()) && (req_msg.GetContentSize() > req_msg.GetClientMaxBodySize()) ) {
		req_msg.SetConnection(false);
		req_msg.SetStatusCode(PAYLOAD_TOO_LARGE);
		return (false);
	}
	return (true);
}
