#ifndef REQUEST_CHUNKED_MESSAGE_HPP
#define REQUEST_CHUNKED_MESSAGE_HPP

#include <string>

class RequestChunkedParser {
   private:
	enum ChunkState {
		CHUNK_START = 0,
		CHUNK_SIZE,
		CHUNK_SIZE_CRLF,
		CHUNK_EXTENSION,
		CHUNK_EXTENSION_NAME,
		CHUNK_EXTENSION_VALUE,
		CHUNK_DATA,
		CHUNK_LASTDATA,
		CHUNK_CRLF,
		CHUNK_TRAILER,
		CHUNK_TRAILER_NAME,
		CHUNK_TRAILER_VALUE,
		CHUNK_TRAILER_CRLF,
		CHUNK_EMPTYLINE,
		CHUNK_ERROR,
		CHUNK_END
	};
	ChunkState parsing_state_;
	bool last_chunk_flag_;
	size_t chunk_size_;
	std::string chunk_size_str_;
	std::string chunk_body_;
	std::string error_msg_;

   public:
	RequestChunkedParser();
	~RequestChunkedParser();
	std::string operator()(const char *str);
	bool IsChunkedDone() const;

   private:
	void ShowParsingState() const;
	ChunkState (RequestChunkedParser::*state_parser_[15])(
		char);
	ChunkState ParseChunk(ChunkState curr_state);
	ChunkState ChunkStart(char c);
	ChunkState ChunkSize(char c);
	ChunkState ChunkSizeCRLF(char c);

	ChunkState ChunkExtension(char c);
	ChunkState ChunkExtensionName(char c);
	ChunkState ChunkExtensionValue(char c);

	ChunkState ChunkData(char c);
	ChunkState ChunkLastData(char c);
	ChunkState ChunkCRLF(char c);

	ChunkState ChunkTrailer(char c);
	ChunkState ChunkTrailerName(char c);
	ChunkState ChunkTrailerValue(char c);
	ChunkState ChunkTrailerCRLF(char c);

	ChunkState ChunkEmptyLine(char c);
	ChunkState Chunk_Error(char c);
};

#endif
