#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <sys/socket.h>

class Message {
   public:
	Message();
	virtual ~Message();

	const std::string GetMessage();

	enum METHOD_TYPE {
		GET,
		POST,
		DELETE
	};

   protected:
	std::string header_;
	std::string body_;
	int method_type_;
};

#endif
