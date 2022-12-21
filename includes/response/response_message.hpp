#ifndef RESPONSE_MESSAGE_HPP
#define RESPONSE_MESSAGE_HPP

#include "request_message.hpp"
#include "status_line.h"
#include "header.h"

class ResponseMessage {
   public:
	std::string ToString();
	ResponseMessage(int status_code, const std::string &reason_phrase);
	void SetBody(const std::string &body);

   private:
	StatusLine status_line_;
	Header headers_;
	std::string body_;
};

std::ostream &operator<<(std::ostream &out, ResponseMessage message);
#endif
