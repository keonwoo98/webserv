#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <sys/socket.h>

class Message {
   public:
	Message();
	virtual ~Message();

	const std::string &GetStartLine() const;
	const std::string &GetHeaders() const;
	const std::string &GetBody() const;

	const std::string GetMessage();

   protected:
	std::string start_line_;
	std::string headers_;
	std::string body_;
};

#endif
