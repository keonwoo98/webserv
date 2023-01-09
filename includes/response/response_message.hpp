#ifndef RESPONSE_MESSAGE_HPP
#define RESPONSE_MESSAGE_HPP

#include "header.hpp"
#include "request_message.hpp"
#include "status_line.hpp"

class ResponseMessage {
   public:
	enum { BUFFER_SIZE = 1024 };

	ResponseMessage();
	ResponseMessage(int status_code, const std::string &reason_phrase);

	const std::string &GetBody() const;
	void SetStatusLine(int status_code, const std::string &reason_phrase);
	bool IsStatusExist();
	void EraseBody(size_t begin, size_t size);
	void ClearBody();

	void SetContentLength();
	void AppendBody(const char *body);
	void AppendBody(const char *body, size_t count);
	void AddCurrentLength(ssize_t send_len);
	void AddLocation(const std::string &uri);
	void AddConnection(const std::string &connection);
	
	void ParseHeader(const std::string &header_line);
	const Header &GetHeader() const;

	bool IsErrorStatus();
	bool IsDone();
	void Clear();

	std::string &ToString();
	std::string GetErrorPagePath(ServerInfo server_info);
	int BodySize();

	ssize_t total_length_;
	ssize_t current_length_;
	std::string raw_data_;
   private:
	StatusLine status_line_;
	Header headers_;
	std::string body_;
};

std::ostream &operator<<(std::ostream &out, ResponseMessage message);
#endif
