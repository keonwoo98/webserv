#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include <iostream>
#include <map>
#include <set>

#define CRLF "\r\n"
#define DCRLF "\r\n\r\n"
#define CR '\r'
#define LF '\n'
#define SP ' '
#define HT '\t'


class RequestMessage {
   public:
	typedef std::map<std::string, std::string> header_map_type;

	RequestMessage();
	~RequestMessage();

	const std::string &GetMethod() const;
	const std::string &GetUri() const;
	const header_map_type &GetHeaderMap() const;
	const std::string &GetBody() const;

	void SetMethod(const std::string &method);
	void SetUri(const std::string &uri);
	void SetHeader(const std::pair<std::string, std::string> &header);
	void SetBody(const std::string &body);

	bool IsThereHost() const;
	void ParsingMessage(const std::string &message);
	class RequestChunkedMessage;

   private:
	std::string method_;
	std::string uri_;
	header_map_type header_map_;
	std::string body_;

	int		content_size_;
	bool	is_chunked_;
	bool	keep_alive_;

	void CheckMethod(const std::string &method) const;
	void CheckUri(const std::string &uri) const;
	void CheckHeader(const std::pair<std::string, std::string> &elem) const;

	void ParsingStartLine(const std::string start_line);
	void ParsingHeaders(const std::string headers);
	void SplitHeader(const std::string &header);
	bool CheckHeader(void) const;
	void ParsingBody(const std::string &body);

	// bool isTchar(char c);
};

class RequestMessage::RequestChunkedMessage
{
private :
	enum ChunkState {
		CHUNK_START = 0,
		CHUNK_SIZE, CHUNK_SIZE_CRLF,
		CHUNK_EXTENSION, CHUNK_EXTENSION_NAME, CHUNK_EXTENSION_VALUE,
		CHUNK_DATA, CHUNK_LASTDATA, CHUNK_CRLF,
		CHUNK_TRAILER, CHUNK_TRAILER_NAME, CHUNK_TRAILER_VALUE, CHUNK_TRAILER_CRLF,
		CHUNK_EMPTYLINE, CHUNK_ERROR,
		CHUNK_END
	};
	bool last_chunk_flag_;
	size_t chunk_size_;
	std::string chunk_size_str_;
	std::string chunk_body_;
	std::string error_msg_;
public :
	RequestChunkedMessage();
	~RequestChunkedMessage();
	std::string operator() (const char * str);
private :
	ChunkState (RequestMessage::RequestChunkedMessage::*parse_state_[15])(char);
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
