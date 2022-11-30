#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include "message.hpp"
#include <iostream>
class RequestMessage : public Message {
   public:
	RequestMessage();
	~RequestMessage();

	void ParsingMessage(const std::string &message);

	private:
};

#endif
