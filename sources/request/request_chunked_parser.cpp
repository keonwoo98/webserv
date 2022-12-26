#include <sstream>
#include <locale> // for isxdigit isalnum

#include "request_parser.hpp"

static RequestState ChunkStart(RequestMessage &req_msg,char c);
static RequestState ChunkSize(RequestMessage &req_msg,char c);
static RequestState ChunkSizeCRLF(RequestMessage &req_msg,char c);

static RequestState ChunkExtension(RequestMessage &req_msg,char c);
static RequestState ChunkExtensionName(RequestMessage &req_msg,char c);
static RequestState ChunkExtensionValue(RequestMessage &req_msg,char c);

static RequestState ChunkData(RequestMessage &req_msg,char c);
static RequestState ChunkLastData(RequestMessage &req_msg,char c);
static RequestState ChunkCRLF(RequestMessage &req_msg,char c);

static RequestState ChunkTrailer(RequestMessage &req_msg,char c);
static RequestState ChunkTrailerName(RequestMessage &req_msg,char c);
static RequestState ChunkTrailerValue(RequestMessage &req_msg,char c);
static RequestState ChunkTrailerCRLF(RequestMessage &req_msg,char c);

static RequestState ChunkEmptyLine(RequestMessage &req_msg,char c);

size_t ParseChunkedRequest(RequestMessage & req_msg, const char * input) {
	size_t count = 0;
	while (*input && req_msg.GetState() != DONE)
	{
		// {
		// 	std::string s = "";
		// 	if (*input == CR) s = "CR";
		// 	else if (*input == LF) s = "LF";
		// 	else s = input[0];
		// 	std::cout << C_PURPLE << "[" << s << "]" << req_msg.GetState() << C_RESET << std::endl;
		// }

		switch (req_msg.GetState())
		{
			case BODY_CHUNK_START : 			req_msg.SetState(ChunkStart(req_msg, *input)); break ;
			case BODY_CHUNK_SIZE : 				req_msg.SetState(ChunkSize(req_msg, *input)); break ;
			case BODY_CHUNK_SIZE_CRLF : 		req_msg.SetState(ChunkSizeCRLF(req_msg, *input)); break ;
			case BODY_CHUNK_EXTENSION : 		req_msg.SetState(ChunkExtension(req_msg, *input)); break ;
			case BODY_CHUNK_EXTENSION_NAME : 	req_msg.SetState(ChunkExtensionName(req_msg, *input)); break ;
			case BODY_CHUNK_EXTENSION_VALUE : 	req_msg.SetState(ChunkExtensionValue(req_msg, *input)); break ;
			case BODY_CHUNK_DATA : 				req_msg.SetState(ChunkData(req_msg, *input)); break ;
			case BODY_CHUNK_LASTDATA : 			req_msg.SetState(ChunkLastData(req_msg, *input)); break ;
			case BODY_CHUNK_CRLF : 				req_msg.SetState(ChunkCRLF(req_msg, *input)); break ;
			case BODY_CHUNK_TRAILER : 			req_msg.SetState(ChunkTrailer(req_msg, *input)); break ;
			case BODY_CHUNK_TRAILER_NAME : 		req_msg.SetState(ChunkTrailerName(req_msg, *input)); break ;
			case BODY_CHUNK_TRAILER_VALUE : 	req_msg.SetState(ChunkTrailerValue(req_msg, *input)); break ;
			case BODY_CHUNK_TRAILER_CRLF : 		req_msg.SetState(ChunkTrailerCRLF(req_msg, *input)); break ;
			case BODY_CHUNK_EMPTYLINE : 		req_msg.SetState(ChunkEmptyLine(req_msg, *input)); break ;
			default : 							req_msg.SetStatusCode(BAD_REQUEST); break;
		}
		
		if (req_msg.GetStatusCode() != CONTINUE)
			req_msg.SetState(DONE);
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
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
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
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

static RequestState ChunkSizeCRLF(RequestMessage &req_msg,char c) {
	if (c != LF)
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
	else
	{
		int chunk_size = hexstrToDec(req_msg.GetChunkSizeStr());
		if (chunk_size == 0)
			return BODY_CHUNK_LASTDATA; // -> 0 혹은 extension 이후에 CRLF를 만나면 LASTDATA로 간다.
		if (chunk_size + req_msg.GetChunkSize() > req_msg.GetClientMaxBodySize()) {
			req_msg.SetStatusCode(PAYLOAD_TOO_LARGE);
			return DONE;
		}
		req_msg.SetChunkSize(chunk_size);
		return BODY_CHUNK_DATA;
	}
}

static RequestState ChunkExtension(RequestMessage &req_msg,char c) {
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

static RequestState ChunkExtensionName(RequestMessage &req_msg,char c) {
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

static RequestState ChunkExtensionValue(RequestMessage &req_msg,char c) {
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

static RequestState ChunkData(RequestMessage &req_msg,char c) {
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

static RequestState ChunkLastData(RequestMessage &req_msg,char c) {
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
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

static RequestState ChunkTrailer(RequestMessage &req_msg,char c) {
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

static RequestState ChunkTrailerName(RequestMessage &req_msg,char c) {
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

static RequestState ChunkTrailerValue(RequestMessage &req_msg,char c) {
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

static RequestState ChunkTrailerCRLF(RequestMessage &req_msg,char c) {
	if (c == LF)
		return BODY_CHUNK_TRAILER;
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}

static RequestState ChunkEmptyLine(RequestMessage &req_msg,char c) {
	if (c == LF)
		return DONE;
	else
	{
		req_msg.SetStatusCode(BAD_REQUEST);
		return BODY_END;
	}
}
