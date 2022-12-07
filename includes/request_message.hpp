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

	void ParsingMessage(const std::string &message);

   private:
	std::string method_;
	std::string uri_;
	header_map_type header_map_;
	std::string body_;
	int		content_size_;
	bool	is_chunked_;
	bool	keep_alive_;

	/* switch 문으로 case by case로 파싱하다보니 이렇게도 가능 할 것 같아서 chunked body class하나 만들어 보겠습니다. */
	class RequestChunkedMessage {
	   private :
		enum state {
			CHUNK_START = 0,
			CHUNK_SIZE, CHUNK_SIZE_CRLF,
			CHUNK_EXTENSION, CHUNK_EXTENSION_NAME, CHUNK_EXTENSION_VALUE,
			CHUNK_DATA, CHUNK_LASTDATA, CHUNK_CRLF,
			CHUNK_TRAILER, CHUNK_TRAILER_NAME, CHUNK_TRAILER_VALUE, CHUNK_TRAILER_CRLF,
			CHUNK_EMPTYLINE, CHUNK_ERROR,
			CHUNK_END,
		};
		bool last_chunk_;
		size_t chunk_size_;
		std::string chunk_size_str_;
		std::string chunk_body_;
		std::string error_msg_;
	   public :
		RequestChunkedMessage();
		~RequestChunkedMessage();
		std::string operator() (const char * str);
		// const std::string & GetBody() const { return chunk_body_; }
		// void ClearBody() { chunk_body_.clear(); }
	   private :
		state (RequestMessage::RequestChunkedMessage::*parse_state_[15])(char);
		state ParseChunk(state curr_state);
		state ChunkStart(char c);
		state ChunkSize(char c);
		state ChunkSizeCRLF(char c);

		state ChunkExtension(char c);
		state ChunkExtensionName(char c);
		state ChunkExtensionValue(char c);
		
		state ChunkData(char c);
		state ChunkLastData(char c);
		state ChunkCRLF(char c);
		
		state ChunkTrailer(char c);
		state ChunkTrailerName(char c);
		state ChunkTrailerValue(char c);
		state ChunkTrailerCRLF(char c);
		
		state ChunkEmptyLine(char c);
		state Chunk_Error(char c);
	};
	RequestChunkedMessage chunk_parser_functor_;

	std::set<std::string> valid_header_name_;
	std::set<std::string> valid_header_value_;

	void ParsingStartLine(const std::string start_line);
	void ParsingHeaders(const std::string headers);
	void SplitHeader(const std::string &header);
	bool CheckHeader(void) const;
	void ParsingBody(const std::string &body);

	bool isTchar(char c);
};

#endif
