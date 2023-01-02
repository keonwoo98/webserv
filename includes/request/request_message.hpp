#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include <iostream>
#include <map>
#include <set>

#include "character_color.hpp"
#include "http_exception.hpp"
#include "request_parsing_state.hpp"
#include "server_info.hpp"
#include "status_code.hpp"

class RequestMessage {
   public:
	typedef std::map<std::string, std::string> headers_type;
	enum { BUFFER_SIZE = 1024 };

	explicit RequestMessage(int c_max_size);
	~RequestMessage();

	void Clear();

	/* GETTER */
	int					GetClientMaxBodySize() const;
	int					GetContentSize() const;
	bool 				IsChunked() const;
	bool				ShouldClose() const;

	RequestState		GetState() const;
	const std::string	&GetHeaderValue(const std::string &key) const;
	const std::string	&GetTempHeaderName() const;
	const std::string	&GetTempHeaderValue() const;
	bool				IsLastChunk() const;
	int					GetChunkSize() const;
	const std::string	&GetChunkSizeStr() const;
	const std::string	&GetChunkBody() const;

	const std::string	&GetMethod() const;
	const std::string	&GetUri() const;
    std::string         &GetUri();
	const std::string	&GetHttpVersion() const;
	const headers_type	&GetHeaders() const;
	const std::string	&GetBody() const;
	std::string			&GetBody();

	const std::string &GetCgiPath() const;
	const std::string &GetResolvedUri() const;
	const std::string &GetQuery() const;
	const std::string &GetServerName() const;


    /* SETTER */
	void SetClientMaxBodySize(int max_size);
	void SetState(RequestState code);
	void SetContentSize(int size);
	void SetChunked(bool is_chunked);
	void SetConnection(bool is_keep_alive);

	void SetLastChunk(bool is_last_chunk);
	void SetChunkSize(size_t size);
    void SetResolvedUri(const std::string &resolvedUri);
    void SetQuery(const std::string &query);
    void ClearChunkSize();
	void ClearChunkSizeStr();
	void ClaerChunkBody();
	/*APPEND*/
	void AppendMethod(char c);
	void AppendUri(char c);
	void AppendProtocol(char c);
	size_t AppendBody(const std::string & str);
	void AppendChunkSizeStr(char c);
	void AppendChunkBody(char c);

	void AppendHeaderName(char c);
	void AppendHeaderValue(char c);
	void AddHeaderField();

   private:
	int client_max_body_size_;

	int content_size_;
	bool is_chunked_;
	bool keep_alive_;

	/* 파싱 과정에서 필요한 정보 */
	RequestState state_;
	std::string temp_header_name_;
	std::string temp_header_value_;
	// for chunked message
	bool last_chunk_flag_;
	int chunk_size_;
	std::string chunk_size_str_;
	std::string chunk_body_;
	std::string error_msg_;

	/* 실질적인 Request Message */
	std::string method_;
	std::string uri_;
	std::string http_version_;
	headers_type headers_;
	std::string body_;

    /* 가공된 Request Message */
    std::string resolved_uri_;
	std::string query_;
	//TODO: is_cgi_ boolean flag 추가하기
    /* TEST 전용 */
public:
    void setUri(const std::string &uri);
	size_t total_length_;
	size_t current_length_;
};

std::ostream &operator<<(std::ostream &os, const RequestMessage &req_msg);

#endif
