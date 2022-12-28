#ifndef HTTP_EXCEPTION
#define HTTP_EXCEPTION

#include <iostream>

class HttpException : public std::exception {
   public:
	HttpException(int status_code, const std::string &message = "unknown");
	~HttpException() throw();
	const char *what() const throw();

	int GetStatusCode() const;
	const std::string &GetReasonPhrase() const;

private:
	std::string err_msg_;
	std::string reason_phrase_;
	int status_code_;
};

#endif
