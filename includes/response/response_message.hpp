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

	std::string ToString();

	void AppendBody(const std::string &body);
	void CalculateLength();
	void AddCurrentLength(int send_len);
	bool HasMore();
	bool IsDone();
	void Clear();

	int total_length_;
	int current_length_;

   private:
	StatusLine status_line_;
	Header headers_;
	std::string body_;
};

std::ostream &operator<<(std::ostream &out, ResponseMessage message);
#endif
