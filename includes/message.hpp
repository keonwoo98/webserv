#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <sys/socket.h>

class Message {
   public:
	Message();
	virtual ~Message();

	void SetHeader(const std::string &header);
	void SetBody(const std::string &body);

	const std::string &GetHeader() const;
	const std::string &GetBody() const;

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
