#include <algorithm>
#include <sstream>

#include "http_exception.hpp"
#include "location_info.hpp"
#include "request_parser.hpp"
#include "server_info.hpp"
#include "status_code.hpp"
#include "util.hpp"

/*
bool isToken(char c);
bool isVChar(char c);
size_t hexstrToDec(std::string hex_string);

void CheckProtocol(RequestMessage & req_msg, const std::string & protocol);
bool CheckHeaderField(const RequestMessage & req_msg);
bool IsThereHost(const RequestMessage & req_msg);
bool CheckSingleHeaderName(const RequestMessage & req_msg);
bool RequestStartlineCheck(RequestMessage & req_msg, const ServerInfo &
server_info); bool RequestHeaderCheck(RequestMessage & req_msg, const ServerInfo
& server_info);
*/

bool isToken(char c) {
	return (c == '!' || c == '#' || c == '$' || c == '%' ||
			c == '&' | c == '\'' || c == '*' || c == '+' || c == '-' ||
			c == '.' || c == '^' || c == '_' || c == '`' || c == '|' ||
			c == '~' || std::isalnum(c));
}

bool isVChar(char c) { return (0x21 <= c && c <= 0x7E); }

size_t hexstrToDec(std::string hex_string) {
	size_t n;

	std::istringstream(hex_string) >> std::hex >> n;
	return n;
}

static bool IsThereHost(const RequestMessage &req_msg);
static bool CheckMethod(RequestMessage &req_msg, const ServerInfo &serverinfo_);
static bool CheckBodySize(RequestMessage &req_msg);
// StartLine에서 프로토콜 다 받자마자 확인
void CheckProtocol(RequestMessage &req_msg, const std::string &protocol) {
	size_t slash_pos = protocol.find('/');
	if (slash_pos == protocol.npos) {
		req_msg.SetConnection(false);
		throw HttpException(BAD_REQUEST,
							"(protocol invalid) : Protocol has no slash(/)");
	}
	std::string http = protocol.substr(0, slash_pos);
	std::string version = protocol.substr(slash_pos + 1);
	if (http != "HTTP") {
		req_msg.SetConnection(false);
		std::string message =
			std::string("(protocol invalid) : Protocol is ") + http;
		throw HttpException(BAD_REQUEST, message.c_str());
	} else if (version != "1.1") {
		req_msg.SetConnection(false);
		std::string message =
			std::string("(protocol invalid) : version is ") + version;
		throw HttpException(HTTP_VERSION_NOT_SUPPORTED, message.c_str());
	}
	return;
}

// 각 headername이 중복인지 확인
void CheckSingleHeaderName(RequestMessage &req_msg) {
	const std::string &target_headername = req_msg.GetTempHeaderName();
	if (target_headername.size() == 0) {
		req_msg.SetConnection(false);
		throw HttpException(BAD_REQUEST,
							"(header validation) : empty header name");
	}
	if (req_msg.GetHeaders().find(target_headername) !=
		req_msg.GetHeaders().end()) {
		req_msg.SetConnection(false);
		throw HttpException(BAD_REQUEST,
							"(header validation) : duplicate header name");
	}
}

// TODO : RequestMessage::GetServerName만들기
// Header를 다 받으면 StartLine, HeaderField 전체적으로 검사
void CheckRequest(RequestMessage &req_msg, ClientSocket *client_socket,
				  const ConfigParser::server_infos_type &server_infos) {
	if (IsThereHost(req_msg) == false) {
		req_msg.SetConnection(false);
		throw HttpException(BAD_REQUEST, "(header invalid) : no host");
	}

	const std::string &server_name = req_msg.GetServerName();
	ConfigParser::server_infos_type::const_iterator it =
		::FindServerInfoToRequestHost(server_name, server_infos);
	client_socket->SetServerInfo(*it);

	const std::string &uri = req_msg.GetUri();
	int location_index = ::FindLocationInfoToUri(uri, *it);
	client_socket->SetLocationIndex(location_index);

	const ServerInfo &target_server_info = client_socket->GetServerInfo();
	size_t max_size = target_server_info.GetClientMaxBodySize();

	req_msg.SetClientMaxBodySize(max_size);

	if (CheckMethod(req_msg, target_server_info) == false) {
		return;
	}
	if (CheckBodySize(req_msg) == false) {
		return;
	}
	if (CheckBodySize(req_msg) == false) {
		return;
	}	
}

void RequestInterimCheck(RequestMessage &req_msg, ClientSocket *client_socket,
						 const ConfigParser::server_infos_type &server_infos) {
	(void)client_socket;
	(void)server_infos;
	if (req_msg.IsChunked() == true) {
		req_msg.SetState(BODY_CHUNK_START);
	} else {
		req_msg.SetState(BODY_NONCHUNK);
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

bool CheckMethod(RequestMessage &req_msg, const ServerInfo &serverinfo_) {
	const std::string &method = req_msg.GetMethod();
	if (serverinfo_.IsAllowedMethod(method) == false) {
		std::string message =
			std::string("(method invalid) : ") + method + " is not allowd";
		throw HttpException(METHOD_NOT_ALLOWED, message);
	}
	if (serverinfo_.IsImplementedMethod(method) == false) {
		std::string message =
			std::string("(method invalid) : ") + method + " is not implemented";
		throw HttpException(NOT_IMPLEMENTED, message);
	}

	const RequestMessage::headers_type &headers = req_msg.GetHeaders();

	RequestMessage::headers_type::const_iterator key;
	key = headers.find("transfer-encoding");
	if (key != headers.end()) {
		if (key->second != "chunked") {
			std::string err_msg(
				"(header invalid) : transfer-encoding is not chunked");
			throw HttpException(BAD_REQUEST, err_msg + " but " + key->second);
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
 * POST일 때, PUT일 때는 사이즈 정보 필요함.
 */
bool CheckBodySize(RequestMessage &req_msg) {
	const RequestMessage::headers_type &header_map = req_msg.GetHeaders();
	RequestMessage::headers_type::const_iterator it = header_map.find("content-length");

	if (it == header_map.end()) { // no content-length header
		if (req_msg.IsChunked() == false) {
			if (req_msg.GetMethod() == "POST" || req_msg.GetMethod() == "PUT") {
				throw HttpException(LENGTH_REQUIRED, "(length invalid) : no content size for POST request");
			}
		}
	} else { // there is content-length header
		if (req_msg.IsChunked() == true) {
			req_msg.SetConnection(false);
		}
	}
	if (req_msg.GetContentSize() > req_msg.GetClientMaxBodySize()) {
		req_msg.SetConnection(false);
		std::string err_msg = "(length invalid) : max client body size is " +
							int_to_str(req_msg.GetClientMaxBodySize()) +
							". input size is " +
							int_to_str(req_msg.GetContentSize());
		throw HttpException(PAYLOAD_TOO_LARGE, err_msg);
	}
	return (true);
}
