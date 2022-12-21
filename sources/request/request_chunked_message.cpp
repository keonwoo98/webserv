#include "request_chunked_message.hpp"
#include "character_const.hpp"
#include <iostream>
#include <sstream>
#include <locale> // for isxdigit isalnum

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

RequestChunkedParser::RequestChunkedParser() {}
RequestChunkedParser::~RequestChunkedParser() {}

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
size_t RequestChunkedParser::operator() (RequestMessage &req_msg, const char * input) {
	size_t count = 0;
	while (*input && req_msg.GetState() != DONE)
	{
		{
			std::string s = "";
			if (*input == CR) s = "CR";
			else if (*input == LF) s = "LF";
			else s = input[0];
			std::cout << C_PURPLE << "[" << s << "]" << req_msg.GetState() << C_RESET << std::endl;
		}

		switch (req_msg.GetState())
		{
			case BODY_CHUNK_START :
				req_msg.SetState(ChunkStart(req_msg, *input));
				break ;
			case BODY_CHUNK_SIZE :
				req_msg.SetState(ChunkSize(req_msg, *input));
				break ;
			case BODY_CHUNK_SIZE_CRLF :
				req_msg.SetState(ChunkSizeCRLF(req_msg, *input));
				break ;
			case BODY_CHUNK_EXTENSION :
				req_msg.SetState(ChunkExtension(req_msg, *input));
				break ;
			case BODY_CHUNK_EXTENSION_NAME :
				req_msg.SetState(ChunkExtensionName(req_msg, *input));
				break ;
			case BODY_CHUNK_EXTENSION_VALUE :
				req_msg.SetState(ChunkExtensionValue(req_msg, *input));
				break ;
			case BODY_CHUNK_DATA :
				req_msg.SetState(ChunkData(req_msg, *input));
				break ;
			case BODY_CHUNK_LASTDATA :
				req_msg.SetState(ChunkLastData(req_msg, *input));
				break ;
			case BODY_CHUNK_CRLF :
				req_msg.SetState(ChunkCRLF(req_msg, *input));
				break ;
			case BODY_CHUNK_TRAILER :
				req_msg.SetState(ChunkTrailer(req_msg, *input));
				break ;
			case BODY_CHUNK_TRAILER_NAME :
				req_msg.SetState(ChunkTrailerName(req_msg, *input));
				break ;
			case BODY_CHUNK_TRAILER_VALUE :
				req_msg.SetState(ChunkTrailerValue(req_msg, *input));
				break ;
			case BODY_CHUNK_TRAILER_CRLF :
				req_msg.SetState(ChunkTrailerCRLF(req_msg, *input));
				break ;
			case BODY_CHUNK_EMPTYLINE :
				req_msg.SetState(ChunkEmptyLine(req_msg, *input));
				break ;
			default :
				req_msg.SetStatusCode(BAD_REQUEST);
				break;
		}
		
		if (req_msg.GetStatusCode() != CONTINUE)
			req_msg.SetState(DONE);
		input++;
		count++;
	}
	return (count);
}

/*
	새로운 청크를 파싱할 준비 한다. size관련 변수를 0으로 초기화 하고 size로 넘어간다.
*/
RequestState 
RequestChunkedParser::ChunkStart (RequestMessage &req_msg, char c)
{
	req_msg.ClearChunkSize();
	req_msg.ClearChunkSizeStr();
	if (std::isxdigit(c) == true)
	{
		req_msg.AppendChunkSizeStr(c);
		return BODY_CHUNK_SIZE;
	}
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

/*
	chunk-size field is string of "hex-digits"
	last-chunk     = 1*("0") [ chunk-ext ] CRLF
	[RFC 9112 7.1]
*/
RequestState 
RequestChunkedParser::ChunkSize (RequestMessage &req_msg, char c)
{
	if (c == ';' || c == SP || c == HT)
		return BODY_CHUNK_EXTENSION;
	else if (c == CR)
		return BODY_CHUNK_SIZE_CRLF;
	else if (::isxdigit(c) == true)
	{
		req_msg.AppendChunkSizeStr(c);
		return BODY_CHUNK_SIZE;
	}
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

/*
	A recipient MUST ignore unrecognized chunk extensions.
	[RFC9112 7.1.1]
	chunk-ext = *( BWS ";" BWS chunk-ext-name[ BWS "=" BWS chunk-ext-val ] )
	; 이전에 LWS skip
	[RFC9112 7.1.2]
*/
RequestState 
RequestChunkedParser::ChunkExtension (RequestMessage &req_msg, char c)
{
	if (c == SP || c == HT)
		return BODY_CHUNK_EXTENSION;
	else if (c == ';')
		return BODY_CHUNK_EXTENSION_NAME;
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

/*
	chunk-ext-name = token
	[RFC9112 7.1.2]
	name뒤에 value가 안 올 수 도 있다. CR만나면 SIZECRLF로 넘어간다.
*/
RequestState 
RequestChunkedParser::ChunkExtensionName (RequestMessage &req_msg, char c)
{
	if (c == SP || c == HT || isToken(c)) // skip
		return BODY_CHUNK_EXTENSION_NAME;
	else if (c == '=')
		return BODY_CHUNK_EXTENSION_VALUE;
	else if (c == CR)
		return BODY_CHUNK_SIZE_CRLF;
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

/* chunk-ext-val  = token / quoted-string [RFC9112 7.1.2] */
RequestState 
RequestChunkedParser::ChunkExtensionValue (RequestMessage &req_msg, char c)
{
	if (c == SP || c == HT || isToken(c)) // skip
		return BODY_CHUNK_EXTENSION_VALUE;
	else if (c == ';')
		return BODY_CHUNK_EXTENSION_NAME;
	else if (c == CR)
		return BODY_CHUNK_SIZE_CRLF;
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

RequestState 
RequestChunkedParser::ChunkSizeCRLF (RequestMessage &req_msg, char c)
{
	if (c != LF)
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
	else
	{
		size_t chunk_size = hexstrToDec(req_msg.GetChunkSizeStr());
		req_msg.SetChunkSize(chunk_size);
		if (req_msg.GetChunkSize() == 0)
			return BODY_CHUNK_LASTDATA; // -> 0 혹은 extension 이후에 CRLF를 만나면 LASTDATA로 간다.
		return BODY_CHUNK_DATA;
	}
}

/* 
 *	chunk-data = 1*OCTET
 *	[RFC9112 7.1]
 *	만약 chunk size랑 CRLF위치가 다르면? -> 명세 못 찾았다. 일단 error처리함.
*/
RequestState 
RequestChunkedParser::ChunkData (RequestMessage &req_msg, char c)
{
	// 남은 chunk_size == 0이면 청크 하나 다 읽음. 올바르게 읽었는지. CRLF확인
	if (req_msg.GetChunkSize() == 0)
	{
		if (c == CR)
			return BODY_CHUNK_CRLF;
		else // RFC에 사이즈와 CLRF가 다르면 어떨 지 안나와있다. 일단 에러처리함.
		{
			req_msg.SetStatusCode(BAD_REQUEST);
			return BODY_END;
		}
	}
	else if (0 <= c && c <= 127)
	{
		req_msg.AppendChunkBody(c);
		req_msg.SetChunkSize(req_msg.GetChunkSize() - 1);
		return BODY_CHUNK_DATA;
	}
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

/*	
	last-chunk = 1*("0") [ chunk-ext ] CRLF
	size가 0인 상태에서 extension이 있든 없든 CRLF를 만나면 여기로 온다.
	여기서 CR이면 EMPTY_LINE, token이면 TRAILER이다.
*/
RequestState 
RequestChunkedParser::ChunkLastData (RequestMessage &req_msg, char c)
{
	req_msg.SetLastChunk(true);
	if (c == CR)
		return BODY_CHUNK_EMPTYLINE;
	else if (isToken(c) == true)
		return BODY_CHUNK_TRAILER_NAME;
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

/*
	하나의 청크가 끝났고, CRLF가 확인 되면 다음 청크를 읽을 준비를 한다.
	last chunk인 경우. body가 없고 size [extension] 뒤의 CRLF이후에
	Trailer 혹은 Emptyline으로 넘어간다.
*/
RequestState 
RequestChunkedParser::ChunkCRLF (RequestMessage &req_msg, char c)
{
	if (c == LF)
	{
		std::cout << C_YELLOW << req_msg.GetChunkBody() << C_RESET << std::endl;
		req_msg.AppendBody(req_msg.GetChunkBody());
		return BODY_CHUNK_START;
	}
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

/*
	trailer-section   = *( field-line CRLF )
	[RFC9112 7.1.2]
	field-line = field-name ":" OWS field-value OWS
	[RFC9112 5]
	trailer가 시작되는 부분.
	첫 문자가 CR이면 CRLF를 확인하기 위해 BODY_CHUNK_EMPTYLINE으로 가고
	첫 문자가 token이라면 TRAILER_NAME으로 간다.
*/
RequestState 
RequestChunkedParser::ChunkTrailer (RequestMessage &req_msg, char c)
{
	if (c == CR)
		return BODY_CHUNK_EMPTYLINE;
	else if (isToken(c) == true)
		return BODY_CHUNK_TRAILER_NAME;
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

/*
	field-name = token
	[RFC9112 5.1]
*/
RequestState 
RequestChunkedParser::ChunkTrailerName (RequestMessage &req_msg, char c)
{
	if (c == ':')
		return BODY_CHUNK_TRAILER_VALUE;
	else if (isToken(c) == true)
		return BODY_CHUNK_TRAILER_NAME;
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

/*
	field-value = *field-content
	field-content = field-vchar
	[ 1*( SP / HTAB / field-vchar ) field-vchar]
	[RFC9112 5.5]
*/
RequestState 
RequestChunkedParser::ChunkTrailerValue (RequestMessage &req_msg, char c)
{
	if (c == SP || c == HT || isVChar(c))
		return BODY_CHUNK_TRAILER_VALUE;
	else if (c == CR)
		return BODY_CHUNK_TRAILER_CRLF;
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

/*
	Trailer section 1줄 받으면 다음 trailer를 받을 수도 empty line을 받을 수도 있다.
	BODY_CHUNK_TRAILER로 가면 이 조건을 확인 할 수 있다.
*/
RequestState 
RequestChunkedParser::ChunkTrailerCRLF (RequestMessage &req_msg, char c)
{
	if (c == LF)
		return BODY_CHUNK_TRAILER;
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}

}

RequestState 
RequestChunkedParser::ChunkEmptyLine (RequestMessage &req_msg, char c)
{
	if (c == LF)
		return DONE;
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}
