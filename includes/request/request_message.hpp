#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include <iostream>
#include <map>
#include <set>

#include "http_exception.hpp"

class RequestMessage {
   public:
	typedef std::map<std::string, std::string> headers_type;

	RequestMessage();
	explicit RequestMessage(int c_max_size);
	~RequestMessage();

	const std::string &GetMethod() const;
	const std::string &GetUri() const;
	const std::string &GetHttpVersion() const;
	const headers_type &GetHeaders() const;
	const std::string &GetBody() const;
	int	GetContentSize() const;

	void SetMethod(const std::string &method);
	void SetUri(const std::string &uri);
	void SetHttpVersion(const std::string &http_version);
	void SetHeader(const std::pair<std::string, std::string> &header);
	void SetBody(const std::string &body);
	void AppendBody(const std::string &body);

	void Clear();

	bool IsThereHost() const;
	bool IsChunked() const;
	class RequestChunkedParser;

   private:
	int client_max_body_size_;
	std::string method_;
	std::string uri_;
	std::string http_version_;
	headers_type headers_;
	std::string body_;

	int content_size_;
	bool is_chunked_;
	bool keep_alive_;

	void CheckMethod(const std::string &method) const;
	void CheckUri(const std::string &uri) const;
	void CheckHttpVersion(const std::string &http_version) const;
	void CheckHeader(const std::pair<std::string, std::string> &header) const;

	size_t CountValue(std::string value) const;
};

#endif
