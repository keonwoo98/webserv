#ifndef HTTP_EXCEPTION
#define HTTP_EXCEPTION

#include <iostream>

class HttpException : public std::exception {
   public:
	HttpException(int status_code);
	const char *what() const throw();

	int GetStatusCode() const;

private:
	int status_code_;
};

#endif
