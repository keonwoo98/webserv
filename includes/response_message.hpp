#ifndef RESPONSE_MESSAGE_HPP
#define RESPONSE_MESSAGE_HPP

#include "message.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <fstream>

class ResponseMessage : public Message {
   public:
	ResponseMessage();
	~ResponseMessage();

	void CreateMessage();
	void SendMessage();

   private:
	void SetHeader();
	void SetBody();

	std::string GetStatus() const;
	std::string GetDate() const;
	std::string GetContentType() const;
	std::string GetContentLength() const;
};

#endif
