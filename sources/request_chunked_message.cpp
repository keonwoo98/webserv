#include "request_message.hpp"
#include <sstream>
#include <fstream>
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

RequestMessage::RequestChunkedMessage::RequestChunkedMessage()
: last_chunk_(false), chunk_body_("") {
	parse_state_[0]  = &RequestMessage::RequestChunkedMessage::ChunkStart;
	parse_state_[1]  = &RequestMessage::RequestChunkedMessage::ChunkSize;
	parse_state_[2]  = &RequestMessage::RequestChunkedMessage::ChunkSizeCRLF;
	parse_state_[3]  = &RequestMessage::RequestChunkedMessage::ChunkExtension;
	parse_state_[4]  = &RequestMessage::RequestChunkedMessage::ChunkExtensionName;
	parse_state_[5]  = &RequestMessage::RequestChunkedMessage::ChunkExtensionValue;
	parse_state_[6]  = &RequestMessage::RequestChunkedMessage::ChunkData;
	parse_state_[7]  = &RequestMessage::RequestChunkedMessage::ChunkLastData;
	parse_state_[8]  = &RequestMessage::RequestChunkedMessage::ChunkCRLF;
	parse_state_[9]  = &RequestMessage::RequestChunkedMessage::ChunkTrailer;
	parse_state_[10] = &RequestMessage::RequestChunkedMessage::ChunkTrailerName;
	parse_state_[11] = &RequestMessage::RequestChunkedMessage::ChunkTrailerValue;
	parse_state_[12] = &RequestMessage::RequestChunkedMessage::ChunkTrailerCRLF;
	parse_state_[13] = &RequestMessage::RequestChunkedMessage::ChunkEmptyLine;
	parse_state_[14] = &RequestMessage::RequestChunkedMessage::Chunk_Error;
}

RequestMessage::RequestChunkedMessage::~RequestChunkedMessage() {}

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
std::string RequestMessage::RequestChunkedMessage::operator() (const char * str) {
	std::string body;
	state parsing_state = CHUNK_START;
	while (*str && parsing_state != CHUNK_END)
		parsing_state = (this->*parse_state_[parsing_state])(*str++);
	body = this->chunk_body_;
	this->chunk_body_.clear();
	return (body);
}

/*
	새로운 청크를 파싱할 준비 한다. size관련 변수를 0으로 초기화 하고 size로 넘어간다.
*/
RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkStart (char c)
{
	this->chunk_size_ = 0;
	this->chunk_size_str_ = "";
	if (std::isdigit(c) == true)
	{
		this->chunk_size_str_.push_back(c);
		return CHUNK_SIZE;
	}
	else
	{
		this->error_msg_ = std::string("Chunk must start with digit");
		return CHUNK_ERROR;
	}
	return (CHUNK_SIZE);
}

/*
	chunk-size field is string of "hex-digits"
	last-chunk     = 1*("0") [ chunk-ext ] CRLF
	[RFC 9112 7.1]
*/
RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkSize (char c)
{
	if (c == ';' || c == SP || c == HT)
		return CHUNK_EXTENSION;
	else if (c == CR)
		return CHUNK_SIZE_CRLF;
	else if (std::isxdigit(c) == true)
	{
		this->chunk_size_str_.push_back(c);
		return CHUNK_SIZE;
	}
	else
	{
		this->error_msg_ = std::string("Chunk size cannot contain ") + c;
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
RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkExtension (char c)
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
RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkExtensionName (char c)
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
RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkExtensionValue (char c)
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

RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkSizeCRLF (char c)
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
RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkData (char c)
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
RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkLastData (char c)
{
	this->last_chunk_ = true;
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
RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkCRLF (char c)
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
RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkTrailer (char c)
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
RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkTrailerName (char c)
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
RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkTrailerValue (char c)
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
RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkTrailerCRLF (char c)
{
	if (c == LF)
		return CHUNK_TRAILER;
	else
	{
		this->error_msg_ = std::string("Bare CR after trailer");
		return CHUNK_ERROR;
	}

}

RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::ChunkEmptyLine (char c)
{
	if (c == LF)
		return CHUNK_END;
	else
	{
		this->error_msg_ = std::string("Bare CR end of the message");
		return CHUNK_ERROR;
	}
}

RequestMessage::RequestChunkedMessage::state
RequestMessage::RequestChunkedMessage::Chunk_Error (char c)
{
	throw std::runtime_error(std::string("Chunk Msg Parse error : ") + error_msg_);
}