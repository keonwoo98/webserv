#ifndef REQUEST_CHUNKED_MESSAGE_HPP
#define REQUEST_CHUNKED_MESSAGE_HPP

#include <string>

#include "request_message.hpp"

class RequestChunkedParser {
   public:
	RequestChunkedParser();
	~RequestChunkedParser();
	size_t operator()(RequestMessage &req_msg, const char *str);
	// bool IsChunkedDone() const;

   private:
	void ShowParsingState() const;
	// void ParseChunk(ChunkState curr_state);
	RequestState ChunkStart(RequestMessage &req_msg, char c);
	RequestState ChunkSize(RequestMessage &req_msg, char c);
	RequestState ChunkSizeCRLF(RequestMessage &req_msg, char c);

	RequestState ChunkExtension(RequestMessage &req_msg, char c);
	RequestState ChunkExtensionName(RequestMessage &req_msg, char c);
	RequestState ChunkExtensionValue(RequestMessage &req_msg, char c);

	RequestState ChunkData(RequestMessage &req_msg, char c);
	RequestState ChunkLastData(RequestMessage &req_msg, char c);
	RequestState ChunkCRLF(RequestMessage &req_msg, char c);

	RequestState ChunkTrailer(RequestMessage &req_msg, char c);
	RequestState ChunkTrailerName(RequestMessage &req_msg, char c);
	RequestState ChunkTrailerValue(RequestMessage &req_msg, char c);
	RequestState ChunkTrailerCRLF(RequestMessage &req_msg, char c);

	RequestState ChunkEmptyLine(RequestMessage &req_msg, char c);
};

#endif
