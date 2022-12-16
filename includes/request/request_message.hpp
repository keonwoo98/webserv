#ifndef REQUESET_MESSAGE_HPP
# define REQUESET_MESSAGE_HPP

#include <string>
#include <sstream>
#include <iostream>

class StartLine;
class Body;

class RequestMessage {
private:
	std::string start_line_; // -> StatusLine
	std::string header_;
	std::string body_;
public:
	explicit RequestMessage(const std::string &http_request) {
		std::stringstream ss(http_request);
		
		std::getline(ss, start_line_);
		size_t index = http_request.find("\r\n\r\n");
		header_ = http_request.substr(start_line_.length() + 1, index - start_line_.length());
		body_ = http_request.substr(index + 4, http_request.length() - (index + 4));

		// std::cout << "index : " << index << std::endl;
		// std::cout << "==========start_line==========" << std::endl;
		// std::cout << start_line_ << std::endl;
		// std::cout << start_line_.length() << std::endl;
		// std::cout << "==========header==========" << std::endl;
		// std::cout << header_ << std::endl;
		// std::cout << header_.length() << std::endl;
		// std::cout << "==========body==========" << std::endl;
		// std::cout << body_ << std::endl;
		// std::cout << body_.length() << std::endl;
	}
};

//  class StartLine {
//  private:
//  	std::string method_;
//  	std::string target_path_;
//  	std::string body_;
//  public:
//  	explicit StartLine(std::string start_line_)
//  	{
//  		start_line_
//  	}
//  };

#endif
