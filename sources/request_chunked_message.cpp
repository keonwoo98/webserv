#include "request_chunked_message.hpp"
#include "request_message.hpp"
#include <iostream>
#include <sstream>
#include <locale> // for isxdigit isalnum

/*
	Delimiters are chosen from the set of 
	US-ASCII visual characters not allowed in a token.
	visual character : 0x21~0x7E
	(DQUOTE and "(),/:;<=>?@[\]{}" are not token)
*/
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

static inline size_t hexstrToDec(std::string hex_string) {
	size_t n;

	std::istringstream(hex_string) >> std::hex >> n;
	return n;
}

RequestChunkedParser::RequestChunkedParser()
: parsing_state_(CHUNK_START), last_chunk_flag_(false), chunk_body_("") {
	state_parser_[0]  = &RequestChunkedParser::ChunkStart;
	state_parser_[1]  = &RequestChunkedParser::ChunkSize;
	state_parser_[2]  = &RequestChunkedParser::ChunkSizeCRLF;
	state_parser_[3]  = &RequestChunkedParser::ChunkExtension;
	state_parser_[4]  = &RequestChunkedParser::ChunkExtensionName;
	state_parser_[5]  = &RequestChunkedParser::ChunkExtensionValue;
	state_parser_[6]  = &RequestChunkedParser::ChunkData;
	state_parser_[7]  = &RequestChunkedParser::ChunkLastData;
	state_parser_[8]  = &RequestChunkedParser::ChunkCRLF;
	state_parser_[9]  = &RequestChunkedParser::ChunkTrailer;
	state_parser_[10] = &RequestChunkedParser::ChunkTrailerName;
	state_parser_[11] = &RequestChunkedParser::ChunkTrailerValue;
	state_parser_[12] = &RequestChunkedParser::ChunkTrailerCRLF;
	state_parser_[13] = &RequestChunkedParser::ChunkEmptyLine;
	state_parser_[14] = &RequestChunkedParser::Chunk_Error;
}

RequestChunkedParser::~RequestChunkedParser() {}

void RequestChunkedParser::ShowParsingState() const
{
	std::string state_string[16] = {
		"CHUNK_START",
		"CHUNK_SIZE",
		"CHUNK_SIZE_CRLF",
		"CHUNK_EXTENSION",
		"CHUNK_EXTENSION_NAME",
		"CHUNK_EXTENSION_VALUE",
		"CHUNK_DATA",
		"CHUNK_LASTDATA",
		"CHUNK_CRLF",
		"CHUNK_TRAILER,",
		"CHUNK_TRAILER_NAME",
		"CHUNK_TRAILER_VALUE",
		"CHUNK_TRAILER_CRLF",
		"CHUNK_EMPTYLINE",
		"CHUNK_ERROR",
		"CHUNK_END",
	};
	// std::cout << "curr pasing state : " << state_string[parsing_state_] << std::endl;
}

/*
    -----------------------------------------------------------
    chunked-body   = *chunk
                     last-chunk
                     trailer-section
                     CRLF
	chunk          = chunk-size [ chunk-ext ] CRLF
                     chunk-data CRLF
    chunk-size     = 1*HEXDIG
    last-chunk     = 1*("0") [ chunk-ext ] CRLF
    chunk-data     = 1*OCTET ; a sequence of chunk-size octets
    -----------------------------------------------------------
	[RFC 9112 7.1]

	 A server ought to limit the total length of chunk extensions
	received in a request to an amount reasonable for the services
	provided,in the same way that it applies length limitations and 
	timeouts for other parts of a message, and generate an appropriate 
	4xx (Client Error) response if that amount is exceeded.
	-> chunk-extension 관련해서는 message에 동일하게 제한을 두어야한다고 한다.
*/
std::string RequestChunkedParser::operator() (const char * str) {
	this->ShowParsingState();
	std::string body = "";
	while (*str && parsing_state_ != CHUNK_END)
	{
		parsing_state_ = (this->*state_parser_[parsing_state_])(*str++);
		if (parsing_state_ == CHUNK_CRLF)
		{
			body += this->chunk_body_;
			this->chunk_body_.clear();
		}
	}
	return (body);
}

bool RequestChunkedParser::IsChunkedDone() const
{
	return (this->parsing_state_ == CHUNK_END);
}

/*
	새로운 청크를 파싱할 준비 한다. size관련 변수를 0으로 초기화 하고 size로 넘어간다.
*/
RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkStart (char c)
{
	this->chunk_size_ = 0;
	this->chunk_size_str_ = "";
	if (std::isxdigit(c) == true)
	{
		this->chunk_size_str_.push_back(c);
		return CHUNK_SIZE;
	}
	else
	{
		this->error_msg_ = std::string("Chunk must start with digit<") + c + ">";
		return CHUNK_ERROR;
	}
	return (CHUNK_SIZE);
}

/*
	chunk-size field is string of "hex-digits"
	last-chunk     = 1*("0") [ chunk-ext ] CRLF
	[RFC 9112 7.1]
*/
RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkSize (char c)
{
	if (c == ';' || c == SP || c == HT)
		return CHUNK_EXTENSION;
	else if (c == CR)
		return CHUNK_SIZE_CRLF;
	else if (::isxdigit(c) == true)
	{
		this->chunk_size_str_.push_back(c);
		return CHUNK_SIZE;
	}
	else
	{
		this->error_msg_ = std::string("Chunk size cannot contain <") + c + ">";
		return CHUNK_ERROR;
	}
}

/*
	A recipient MUST ignore unrecognized chunk extensions.
	[RFC9112 7.1.1]
	chunk-ext = *( BWS ";" BWS chunk-ext-name[ BWS "=" BWS chunk-ext-val ] )
	; 이전에 LWS skip
	[RFC9112 7.1.2]
*/
RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkExtension (char c)
{
	if (c == SP || c == HT)
		return CHUNK_EXTENSION;
	else if (c == ';')
		return CHUNK_EXTENSION_NAME;
	else
	{
		this->error_msg_ = std::string("Invalid character in chunk extension");
		return CHUNK_ERROR;
	}
}

/*
	chunk-ext-name = token
	[RFC9112 7.1.2]
	name뒤에 value가 안 올 수 도 있다. CR만나면 SIZECRLF로 넘어간다.
*/
RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkExtensionName (char c)
{
	if (c == SP || c == HT || isToken(c)) // skip
		return CHUNK_EXTENSION_NAME;
	else if (c == '=')
		return CHUNK_EXTENSION_VALUE;
	else if (c == CR)
		return CHUNK_SIZE_CRLF;
	else
	{
		this->error_msg_ = std::string("Invalid character in chunk extension name");
		return CHUNK_ERROR;
	}
}

/* chunk-ext-val  = token / quoted-string [RFC9112 7.1.2] */
RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkExtensionValue (char c)
{
	if (c == SP || c == HT || isToken(c)) // skip
		return CHUNK_EXTENSION_VALUE;
	else if (c == ';')
		return CHUNK_EXTENSION_NAME;
	else if (c == CR)
		return CHUNK_SIZE_CRLF;
	else
	{
		this->error_msg_ = std::string("Invalid character in chunk extension value");
		return CHUNK_ERROR;
	}
}

RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkSizeCRLF (char c)
{
	if (c != LF)
	{
		this->error_msg_ = std::string("Bare CR after size");
		return CHUNK_ERROR;
	}
	else
	{
		chunk_size_ = hexstrToDec(chunk_size_str_);
		if (chunk_size_ == 0)
			return CHUNK_LASTDATA; // -> 0 혹은 extension 이후에 CRLF를 만나면 LASTDATA로 간다.
		return CHUNK_DATA;
	}
}

/* 
 *	chunk-data = 1*OCTET
 *	[RFC9112 7.1]
 *	만약 chunk size랑 CRLF위치가 다르면? -> 명세 못 찾았다. 일단 error처리함.
*/
RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkData (char c)
{
	// 남은 chunk_size == 0이면 청크 하나 다 읽음. 올바르게 읽었는지. CRLF확인
	if (this->chunk_size_ == 0)
	{
		if (c == CR)
			return CHUNK_CRLF;
		else // RFC에 사이즈와 CLRF가 다르면 어떨 지 안나와있다. 일단 에러처리함.
		{
			this->error_msg_ = std::string("Chunk size doesn't match with actual body size");
			return CHUNK_ERROR;
		} 
	} 
	else if (0 <= c && c <= 127)
	{
		this->chunk_body_.push_back(c);
		chunk_size_--;
		return CHUNK_DATA;
	}
	else
	{
		this->error_msg_ = std::string("invalid character in body");
		return CHUNK_ERROR;
	}
}

/*	
	last-chunk = 1*("0") [ chunk-ext ] CRLF
	size가 0인 상태에서 extension이 있든 없든 CRLF를 만나면 여기로 온다.
	여기서 CR이면 EMPTY_LINE, token이면 TRAILER이다.
*/
RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkLastData (char c)
{
	this->last_chunk_flag_ = true;
	if (c == CR)
		return CHUNK_EMPTYLINE;
	else if (isToken(c) == true)
		return CHUNK_TRAILER_NAME;
	else
	{
		this->error_msg_ = std::string("Bare CR end of the body");
		return CHUNK_ERROR;
	}
}

/*
	하나의 청크가 끝났고, CRLF가 확인 되면 다음 청크를 읽을 준비를 한다.
	last chunk인 경우. body가 없고 size [extension] 뒤의 CRLF이후에
	Trailer 혹은 Emptyline으로 넘어간다.
*/
RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkCRLF (char c)
{
	if (c == LF)
		return CHUNK_START;
	else
	{
		this->error_msg_ = std::string("Bare CR end of the body");
		return CHUNK_ERROR;
	}
}

/*
	trailer-section   = *( field-line CRLF )
	[RFC9112 7.1.2]
	field-line = field-name ":" OWS field-value OWS
	[RFC9112 5]
	trailer가 시작되는 부분.
	첫 문자가 CR이면 CRLF를 확인하기 위해 CHUNK_EMPTYLINE으로 가고
	첫 문자가 token이라면 TRAILER_NAME으로 간다.
*/
RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkTrailer (char c)
{
	if (c == CR)
		return CHUNK_EMPTYLINE;
	else if (isToken(c) == true)
		return CHUNK_TRAILER_NAME;
	else
	{
		this->error_msg_ = std::string("Chunk trailer syntax error");
		return CHUNK_ERROR;
	}
}

/*
	field-name = token
	[RFC9112 5.1]
*/
RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkTrailerName (char c)
{
	if (c == ':')
		return CHUNK_TRAILER_VALUE;
	else if (isToken(c) == true)
		return CHUNK_TRAILER_NAME;
	else
	{
		this->error_msg_ = std::string("Chunk trailer syntax error");
		return CHUNK_ERROR;
	}
}

/*
	field-value = *field-content
	field-content = field-vchar
	[ 1*( SP / HTAB / field-vchar ) field-vchar]
	[RFC9112 5.5]
*/
RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkTrailerValue (char c)
{
	if (c == SP || c == HT || isVChar(c))
		return CHUNK_TRAILER_VALUE;
	else if (c == CR)
		return CHUNK_TRAILER_CRLF;
	else
	{
		this->error_msg_ = std::string("Chunk trailer syntax error");
		return CHUNK_ERROR;
	}
}

/*
	Trailer section 1줄 받으면 다음 trailer를 받을 수도 empty line을 받을 수도 있다.
	CHUNK_TRAILER로 가면 이 조건을 확인 할 수 있다.
*/
RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkTrailerCRLF (char c)
{
	if (c == LF)
		return CHUNK_TRAILER;
	else
	{
		this->error_msg_ = std::string("Bare CR after trailer");
		return CHUNK_ERROR;
	}

}

RequestChunkedParser::ChunkState
RequestChunkedParser::ChunkEmptyLine (char c)
{
	if (c == LF)
		return CHUNK_END;
	else
	{
		this->error_msg_ = std::string("Bare CR end of the message");
		return CHUNK_ERROR;
	}
}

RequestChunkedParser::ChunkState
RequestChunkedParser::Chunk_Error (char c)
{
	(void)c; // for unused parameter
	throw std::runtime_error(std::string("Chunk Msg Parse error : ") + error_msg_);
}
