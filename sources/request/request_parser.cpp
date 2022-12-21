#include <algorithm>  // for std::transform
#include <sstream>
#include <locale>
#include <vector>

#include "character_color.hpp"
#include "request_parser.hpp"
#include "request_message.hpp"
#include "character_const.hpp"

static inline bool isToken(char c) {
	return (c == '!' || c == '#' || c ==  '$' || c ==  '%' || c ==  '&' \
	| c ==  '\'' || c ==  '*' || c ==  '+' || c ==  '-' || c ==  '.' \
	|| c ==  '^' || c ==  '_' || c ==  '`' || c ==  '|' || c ==  '~' \
	|| std::isalnum(c));
}

static inline bool isVChar(char c)
{
	return (0x21 <= c && c <= 0x7E);
}

RequestParser::RequestParser() {}

RequestParser::~RequestParser() {}

/********************/
/*      PARSER      */
/********************/

void RequestParser::operator() (RequestMessage & req_msg, const char * input)
{
	while (*input != '\0' && req_msg.GetState() != DONE)
	{
		RequestState curr_state = req_msg.GetState();
		if (START_METHOD <= curr_state && curr_state <= START_END)
			ParseStartLine(req_msg, *input++);
		else if (HEADER_NAME <= curr_state && curr_state <= HEADER_END)
			ParseHeader(req_msg, *input++);
		else if (BODY_BEGIN <= curr_state && curr_state <= BODY_END)
			input += ParseBody(req_msg, input);

		if (req_msg.GetStatusCode() != CONTINUE)
			req_msg.SetState(DONE);
	}
}

void RequestParser::ParseStartLine(RequestMessage & req_msg, char c)
{
	switch (req_msg.GetState())
	{
		case START_METHOD :
			if (isupper(c) == true)
				req_msg.AppendMethod(c);
			else if (c == SP)
				req_msg.SetState(START_URI);
			else {
				req_msg.SetStatusCode(BAD_REQUEST);
				req_msg.SetConnection(false);
			}
			break;
		case START_URI :
			if (isVChar(c) == true)
				req_msg.AppendUri(c);
			else if (c == SP)
				req_msg.SetState(START_PROTOCOL) ;
			else
				req_msg.SetStatusCode(BAD_REQUEST);
			break;
		case START_PROTOCOL :
			if (isVChar(c) == true)
				req_msg.AppendProtocol(c);
			else if (c == CR)
				req_msg.SetState(START_END) ;
			else
				req_msg.SetStatusCode(BAD_REQUEST);
			break;
		case START_END :
			if (c != LF)
				req_msg.SetStatusCode(BAD_REQUEST);
			else {
				CheckProtocol(req_msg, req_msg.GetHttpVersion());
				req_msg.SetState(HEADER_NAME);
			}
			break;
		default :
			req_msg.SetStatusCode(BAD_REQUEST);
			break ;
	}
}

void RequestParser::ParseHeader(RequestMessage & req_msg, char c)
{
	switch (req_msg.GetState())
	{
		case HEADER_NAME :
			if (isToken(c) == true)
				req_msg.AppendHeaderName(tolower(c));
			else if (c == COLON)
				req_msg.SetState(HEADER_COLON);
			else
				req_msg.SetStatusCode(BAD_REQUEST);
			break;
		case HEADER_COLON :
			if (CheckSingleHeaderName(req_msg))
				req_msg.SetState(HEADER_SP_AFTER_COLON);
			else
				req_msg.SetStatusCode(BAD_REQUEST);
			break;
		case HEADER_SP_AFTER_COLON :
			if (c == SP)
				req_msg.SetState(HEADER_SP_AFTER_COLON);
			else if (isVChar(c) == true) {
				req_msg.AppendHeaderValue(c);
				req_msg.SetState(HEADER_VALUE);
			}
			else
				req_msg.SetStatusCode(BAD_REQUEST);
			break;
		case HEADER_VALUE :
			if (c == CR)
				req_msg.SetState(HEADER_CR);
			else if (isVChar(c) == true || c == SP || c == HT)
				req_msg.AppendHeaderValue(c);
			else
				req_msg.SetStatusCode(BAD_REQUEST);
			break;
		case HEADER_CR : // 헤더 한 줄이 완료되는 부분
			req_msg.AddHeaderField();
			if (c == LF)
				req_msg.SetState(HEADER_CRLF);
			else
				req_msg.SetStatusCode(BAD_REQUEST);
			break;
		case HEADER_CRLF :
			if (c == CR)
				req_msg.SetState(HEADER_END);
			else if (isalpha(c) == true) {
				req_msg.AppendHeaderName(tolower(c));
				req_msg.SetState(HEADER_NAME);
			}
			else
				req_msg.SetStatusCode(BAD_REQUEST);
			break;
		case HEADER_END : // 헤더 전체가 완료되는 부분
			if (c != LF) {
				req_msg.SetStatusCode(BAD_REQUEST);
			} else if (req_msg.GetMethod() == "POST" && req_msg.IsChunked() == false) {
				req_msg.SetState(BODY_NONCHUNK);
			} else if (req_msg.GetMethod() == "POST" && req_msg.IsChunked() == true) {
				req_msg.SetState(BODY_CHUNK_START);
			} else {
				req_msg.SetState(DONE);
			}
			break;
		default :
			break ;
	}
}

size_t RequestParser::ParseBody(RequestMessage & req_msg, const char * input)
{
	if (req_msg.IsChunked() == false) {
		if (req_msg.GetContentSize() == -1) {
			req_msg.SetConnection(false);
			req_msg.SetStatusCode(LENGTH_REQUIRED);
			return (0);
		} else {
			return (ParseUnchunkedBody(req_msg, input));
		}
	} else {
		if (req_msg.GetContentSize() != -1) 
			req_msg.SetConnection(false);
		std::cout << "CHUNK PARSE" << std::endl;
		return (ParseChunkedBody(req_msg, input));
	}
}

size_t RequestParser::ParseUnchunkedBody(RequestMessage & req_msg, const char * input)
{
	std::string buffer = input;
	size_t size;
	int size_left = req_msg.GetContentSize() - req_msg.GetBody().size();
	
	if (size_left <= (int)buffer.size()) {
		size = req_msg.AppendBody(buffer.substr(0, size_left));
		req_msg.SetState(DONE);
		return (size);
	} else {
		return (req_msg.AppendBody(buffer));
	}
}

size_t RequestParser::ParseChunkedBody(RequestMessage & req_msg, const char * input)
{
	return (chunked_parser_(req_msg, input));
}


/********************/
/*     CHECKER      */
/********************/

void RequestParser::CheckProtocol(RequestMessage & req_msg, const std::string & protocol)
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

bool RequestParser::CheckSingleHeaderName(const RequestMessage & req_msg) const {
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
	if (req_msg.GetHeaders().find("host") != req_msg.GetHeaders().end()){
		req_msg.SetStatusCode(BAD_REQUEST);
		return (false);
	}

	return (true);
}
