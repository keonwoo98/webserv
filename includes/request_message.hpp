#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include <iostream>
#include <map>

class RequestMessage {
   public:
	RequestMessage();
	~RequestMessage();

	void ParsingMessage(const std::string &message);

   private:
	std::string method_;
	std::string uri_;
	std::map<std::string, std::string> header_map_;
	std::string body_;

	void ParsingStartLine(const std::string start_line);
	void ParsingHeaders(const std::string headers);
	void SplitHeader(const std::string &header);
};

#endif
