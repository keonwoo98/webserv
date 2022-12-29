#include <locale> // for isxdigit isalnum

#include "character_const.hpp"
#include "request_parser.hpp"
#include "http_exception.hpp"

static void ParseStartLine(RequestMessage & req_msg, char c);
static void ParseHeader(RequestMessage & req_msg, char c);
static size_t ParseBody(RequestMessage & req_msg, const char * input);
static size_t ParseUnchunkedBody(RequestMessage & req_msg, const char * input);

void ParseRequest(RequestMessage & req_msg,
					ClientSocket *client_socket,
					const ConfigParser::server_infos_type &server_infos,
					const char * input)
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

		if (req_msg.GetState() == HEADER_CHECK)
			CheckRequest(req_msg, client_socket);
	}
}

static void ParseStartLine(RequestMessage & req_msg, char c)
{
	switch (req_msg.GetState())
	{
		case START_METHOD :
			if (isupper(c) == true)
				req_msg.AppendMethod(c);
			else if (c == SP)
				req_msg.SetState(START_URI);
			else {
				req_msg.SetConnection(false);
				throw HttpException(BAD_REQUEST,
					"(request startline) : syntax error. invalid char for METHOD");
			}
			break;
		case START_URI :
			if (isVChar(c) == true)
				req_msg.AppendUri(c);
			else if (c == SP)
				req_msg.SetState(START_PROTOCOL) ;
			else
				throw HttpException(BAD_REQUEST,
					"(request startline) : syntax error. invalid char for URI");
			break;
		case START_PROTOCOL :
			if (isVChar(c) == true)
				req_msg.AppendProtocol(c);
			else if (c == CR)
				req_msg.SetState(START_END) ;
			else
				throw HttpException(BAD_REQUEST,
					"(request startline) : syntax error. invalid char for PROTOCOL");
			break;
		case START_END :
			if (c != LF)
				throw HttpException(BAD_REQUEST,
					"(request startline) : syntax error. bare CR not allowed in start line");
			else {
				CheckProtocol(req_msg, req_msg.GetHttpVersion());
				req_msg.SetState(HEADER_NAME);
			}
			break;
		default :
			throw HttpException(BAD_REQUEST, 
				"(request startline) : unknown");
			break ;
	}
}

static void ParseHeader(RequestMessage & req_msg, char c)
{
	switch (req_msg.GetState())
	{
		case HEADER_NAME :
			if (isToken(c) == true)
				req_msg.AppendHeaderName(tolower(c));
			else if (c == COLON)
				req_msg.SetState(HEADER_COLON);
			else
				throw HttpException(BAD_REQUEST, 
					"(request header) : syntax error. invalid char for header name");
			break;
		case HEADER_COLON : // 헤더 name이 다 읽힘
			CheckSingleHeaderName(req_msg);
			req_msg.SetState(HEADER_SP_AFTER_COLON);
			break;
		case HEADER_SP_AFTER_COLON :
			if (c == SP)
				req_msg.SetState(HEADER_SP_AFTER_COLON);
			else if (isVChar(c) == true) {
				req_msg.AppendHeaderValue(c);
				req_msg.SetState(HEADER_VALUE);
			}
			else
				throw HttpException(BAD_REQUEST,
					"(request header) : syntax error. invalid char for header value");
			break;
		case HEADER_VALUE :
			if (c == CR)
				req_msg.SetState(HEADER_CR);
			else if (isVChar(c) == true || c == SP || c == HT)
				req_msg.AppendHeaderValue(c);
			else
				throw HttpException(BAD_REQUEST,
					"(request header) : syntax error. invalid char for header value");
			break;
		case HEADER_CR : // 헤더 한 줄이 완료되는 부분
			req_msg.AddHeaderField();
			if (c == LF)
				req_msg.SetState(HEADER_CRLF);
			else
				throw HttpException(BAD_REQUEST,
					"(request header) : syntax error. bare CR not allowed in header field,");
			break;
		case HEADER_CRLF :
			if (c == CR)
				req_msg.SetState(HEADER_END);
			else if (isalpha(c) == true) {
				req_msg.AppendHeaderName(tolower(c));
				req_msg.SetState(HEADER_NAME);
			}
			else
				throw HttpException(BAD_REQUEST,
					"(request header) : syntax error. invalid char for header field");
			break;
		case HEADER_END : // 헤더 전체가 완료되는 부분
			if (c == LF) {
				req_msg.SetState(HEADER_CHECK);
			} else {
				throw HttpException(BAD_REQUEST,
					"(request header) : syntax error. bare CR not allowed in header field");
			}
		default :
			break ;
	}
}

static size_t ParseBody(RequestMessage & req_msg, const char * input)
{
	if (req_msg.IsChunked() == false) {
		return (ParseUnchunkedBody(req_msg, input));
	} else {
		return (ParseChunkedRequest(req_msg, input));
	}
}

static size_t ParseUnchunkedBody(RequestMessage & req_msg, const char * input)
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
