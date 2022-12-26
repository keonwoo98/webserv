#ifndef RESPONSE_MESSAGE_HPP
#define RESPONSE_MESSAGE_HPP

#include "header.h"
#include "request_message.hpp"
#include "status_line.h"

class ResponseMessage {
   public:
	ResponseMessage();
	ResponseMessage(int status_code, const std::string &reason_phrase);

	std::string ToString();

	void SetBody(const std::string &body);
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
