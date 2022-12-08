#ifndef REQUEST_EXCEPTION
#define REQUEST_EXCEPTION

#include <iostream>

class RequestException {
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
