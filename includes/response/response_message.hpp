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

	void AppendBody(const char *body);
	void AppendBody(const char *body, size_t count);
	void AddCurrentLength(int send_len);
	void AddLocation(const std::string &uri);
	void AddConnection(const std::string &connection);
	
	bool IsErrorStatus();
	bool IsDone();
	void Clear();

	void SetStatusLine(int status_code, const std::string &reason_phrase);

	std::string ToString();
	std::string GetErrorPagePath(ServerInfo server_info);
	int BodySize();

	int total_length_;
	int current_length_;
   private:
	StatusLine status_line_;
	Header headers_;
	std::string body_;
};

std::ostream &operator<<(std::ostream &out, ResponseMessage message);
#endif
