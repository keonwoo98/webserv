#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include "message.hpp"
#include <iostream>
class RequestMessage : public Message {
   public:
	RequestMessage();
	~RequestMessage();

	enum METHOD_TYPE {
		GET,
		POST,
		DELETE
	};

	void SetMessage(const std::string &message);
	void ParsingMessage();
};

#endif
