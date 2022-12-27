#include <sstream>
#include <locale> // for isxdigit isalnum

#include "request_parser.hpp"
#include "http_exception.hpp"
#include "util.hpp"

static RequestState ChunkStart(RequestMessage &req_msg,char c);
static RequestState ChunkSize(RequestMessage &req_msg,char c);
static RequestState ChunkSizeCRLF(RequestMessage &req_msg,char c);

static RequestState ChunkExtension(char c);
static RequestState ChunkExtensionName(char c);
static RequestState ChunkExtensionValue(char c);

static RequestState ChunkData(RequestMessage &req_msg,char c);
static RequestState ChunkLastData(RequestMessage &req_msg,char c);
static RequestState ChunkCRLF(RequestMessage &req_msg,char c);

static RequestState ChunkTrailer(char c);
static RequestState ChunkTrailerName(char c);
static RequestState ChunkTrailerValue(char c);
static RequestState ChunkTrailerCRLF(char c);

static RequestState ChunkEmptyLine(char c);

size_t ParseChunkedRequest(RequestMessage & req_msg, const char * input) {
	size_t count = 0;
	while (*input && req_msg.GetState() != DONE)
	{
		switch (req_msg.GetState())
		{
			case BODY_CHUNK_START :
				req_msg.SetState(ChunkStart(req_msg, *input)); break ;
			case BODY_CHUNK_SIZE :
				req_msg.SetState(ChunkSize(req_msg, *input)); break ;
			case BODY_CHUNK_SIZE_CRLF :
				req_msg.SetState(ChunkSizeCRLF(req_msg, *input)); break ;
			case BODY_CHUNK_EXTENSION :
				req_msg.SetState(ChunkExtension(*input)); break ;
			case BODY_CHUNK_EXTENSION_NAME :
				req_msg.SetState(ChunkExtensionName(*input)); break ;
			case BODY_CHUNK_EXTENSION_VALUE :
				req_msg.SetState(ChunkExtensionValue(*input)); break ;
			case BODY_CHUNK_DATA :
				req_msg.SetState(ChunkData(req_msg, *input)); break ;
			case BODY_CHUNK_LASTDATA :
				req_msg.SetState(ChunkLastData(req_msg, *input)); break ;
			case BODY_CHUNK_CRLF :
				req_msg.SetState(ChunkCRLF(req_msg, *input)); break ;
			case BODY_CHUNK_TRAILER :
				req_msg.SetState(ChunkTrailer(*input)); break ;
			case BODY_CHUNK_TRAILER_NAME :
				req_msg.SetState(ChunkTrailerName(*input)); break ;
			case BODY_CHUNK_TRAILER_VALUE :
				req_msg.SetState(ChunkTrailerValue(*input)); break ;
			case BODY_CHUNK_TRAILER_CRLF :
				req_msg.SetState(ChunkTrailerCRLF(*input)); break ;
			case BODY_CHUNK_EMPTYLINE :
				req_msg.SetState(ChunkEmptyLine(*input)); break ;
			default :
				throw HttpException(BAD_REQUEST, "(chunked parser) : unknown");
				break;
		}
		input++;
		count++;
	}
	return (count);
}

static RequestState ChunkStart(RequestMessage &req_msg,char c) {
	req_msg.ClearChunkSize();
	req_msg.ClearChunkSizeStr();
	if (std::isxdigit(c) == true)
	{
		req_msg.AppendChunkSizeStr(c);
		return BODY_CHUNK_SIZE;
	}
	else
		throw HttpException(BAD_REQUEST, 
			"(chunked parser) : syntax error. chunk size should be hex-digit");
}

static RequestState ChunkSize(RequestMessage &req_msg,char c) {
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
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : syntax error. chunk size should be hex-digit");
}

static RequestState ChunkSizeCRLF(RequestMessage &req_msg,char c) {
	if (c != LF)
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : syntax error. bare CR not allowed in chunk size line");
	else
	{
		int chunk_size = hexstrToDec(req_msg.GetChunkSizeStr());
		if (chunk_size == 0)
			return BODY_CHUNK_LASTDATA; // -> 0 혹은 extension 이후에 CRLF를 만나면 LASTDATA로 간다.
		if (chunk_size + req_msg.GetChunkSize() > req_msg.GetClientMaxBodySize()) {
			std::string err_msg = "(chunked parser) : max client body size is "\
								+ int_to_str(req_msg.GetClientMaxBodySize())\
								+ ". input size is "\
								+ int_to_str(chunk_size + req_msg.GetChunkSize());
			throw HttpException(PAYLOAD_TOO_LARGE, err_msg.c_str());
		}
		req_msg.SetChunkSize(chunk_size);
		return BODY_CHUNK_DATA;
	}
}

static RequestState ChunkExtension(char c) {
	if (c == SP || c == HT)
		return BODY_CHUNK_EXTENSION;
	else if (c == ';')
		return BODY_CHUNK_EXTENSION_NAME;
	else
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : syntax error. ");
}

static RequestState ChunkExtensionName(char c) {
	if (c == SP || c == HT || isToken(c)) // skip
		return BODY_CHUNK_EXTENSION_NAME;
	else if (c == '=')
		return BODY_CHUNK_EXTENSION_VALUE;
	else if (c == CR)
		return BODY_CHUNK_SIZE_CRLF;
	else
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : chunk extension name error");
}

static RequestState ChunkExtensionValue(char c) {
	if (c == SP || c == HT || isToken(c)) // skip
		return BODY_CHUNK_EXTENSION_VALUE;
	else if (c == ';')
		return BODY_CHUNK_EXTENSION_NAME;
	else if (c == CR)
		return BODY_CHUNK_SIZE_CRLF;
	else
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : chunk extension value error");
}

static RequestState ChunkData(RequestMessage &req_msg,char c) {
	// 남은 chunk_size == 0이면 청크 하나 다 읽음. 올바르게 읽었는지. CRLF확인
	if (req_msg.GetChunkSize() == 0)
	{
		if (c == CR)
			return BODY_CHUNK_CRLF;
		else // RFC에 사이즈와 CLRF가 다르면 어떨 지 안나와있다. 일단 에러처리함.
			throw HttpException(BAD_REQUEST,
				"(chunked parser) : chunk size differs to actual chunk body size");
	}
	else if (0 <= c && c <= 127)
	{
		req_msg.AppendChunkBody(c);
		req_msg.SetChunkSize(req_msg.GetChunkSize() - 1);
		return BODY_CHUNK_DATA;
	}
	else
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : syntax error. invalid char for chunk data");
}

static RequestState ChunkLastData(RequestMessage &req_msg,char c) {
	req_msg.SetLastChunk(true);
	if (c == CR)
		return BODY_CHUNK_EMPTYLINE;
	else if (isToken(c) == true)
		return BODY_CHUNK_TRAILER_NAME;
	else
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : syntax error. invalid char for last chunk");
}

static RequestState ChunkCRLF(RequestMessage &req_msg,char c) {
	if (c == LF)
	{
		req_msg.AppendBody(req_msg.GetChunkBody());
		req_msg.ClearChunkSize();
		req_msg.ClearChunkSizeStr();
		req_msg.ClaerChunkBody();
		std::cout << C_YELLOW << "new size: " << req_msg.GetContentSize() << std::endl;
		return BODY_CHUNK_START;
	}
	else
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : syntax error. bare CR not allowd in chunk body");
}

static RequestState ChunkTrailer(char c) {
	if (c == CR)
		return BODY_CHUNK_EMPTYLINE;
	else if (isToken(c) == true)
		return BODY_CHUNK_TRAILER_NAME;
	else
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : syntax error. invalid char for chunk trailer");
}

static RequestState ChunkTrailerName(char c) {
	if (c == ':')
		return BODY_CHUNK_TRAILER_VALUE;
	else if (isToken(c) == true)
		return BODY_CHUNK_TRAILER_NAME;
	else
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : syntax error. invalid char for chunk trailer name");
}

static RequestState ChunkTrailerValue(char c) {
	if (c == SP || c == HT || isVChar(c))
		return BODY_CHUNK_TRAILER_VALUE;
	else if (c == CR)
		return BODY_CHUNK_TRAILER_CRLF;
	else
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : syntax error. invalid char for chunk trailer value");
}

static RequestState ChunkTrailerCRLF(char c) {
	if (c == LF)
		return BODY_CHUNK_TRAILER;
	else
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : syntax error. bare CR not allowed in chunk trailer");
}

static RequestState ChunkEmptyLine(char c) {
	if (c == LF)
		return DONE;
	else
		throw HttpException(BAD_REQUEST,
			"(chunked parser) : syntax error. bare CR not allowed in last empty line");
}
