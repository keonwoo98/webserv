#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include "message.hpp"
#include <iostream>
class RequestMessage : public Message {
   public:
	RequestMessage();
	~RequestMessage();

	int RecvMessage(int fd);
	void ParsingMessage();
};

#endif
