#ifndef HTTP_EXCEPTION
#define HTTP_EXCEPTION

#include <iostream>

class HttpException {
   public:
	class E400 : public std::exception {
	   public:
		const char *what() const throw();
	};
	class E401 : public std::exception {
	   public:
		const char *what() const throw();
	};
	class E501 : public std::exception {
	   public:
		const char *what() const throw();
	};
};

#endif
