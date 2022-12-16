#ifndef RESPONSE_MESSAGE_HPP
#define RESPONSE_MESSAGE_HPP

#include "request_message.hpp"
#include "uri.hpp"

class ResponseMessage {
   public:
	ResponseMessage(RequestMessage &request);
	~ResponseMessage();

	const std::string &GetStartLine() const;
	const std::string &GetHeaders() const;
	const std::string &GetBody() const;

	const std::string GetMessage();

	void CreateMessage();

   private:
	void CreateHeader();
	void CreateBody();

	std::string GetStatus() const;
	std::string GetDate() const;
	std::string GetContentType() const;
	std::string GetContentLength() const;

   private:
	RequestMessage &request_;

	std::string start_line_;
	std::string headers_;
	std::string body_;
};

#endif
