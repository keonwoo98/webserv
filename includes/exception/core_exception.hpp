//
// Created by 김민준 on 2022/12/20.
//

#ifndef WEBSERV_INCLUDES_CORE_CORE_EXCEPTION_HPP
#define WEBSERV_INCLUDES_CORE_CORE_EXCEPTION_HPP

#include <exception>

class CoreException : public std::exception {
   public:
	class ListenException : public std::exception {
		const char *what() const throw() { return "listen() Failed"; }
	};

	class BindException : public std::exception {
		const char *what() const throw() { return "bind() Failed"; }
	};

	class GetAddrInfoException : public std::exception {
		const char *what() const throw() { return "getaddrinfo() Failed"; }
	};

	class KQueueException : public std::exception {
		const char *what() const throw() { return "kqueue() failed"; }
	};

	class FileOpenException : public std::exception {
		const char *what() const throw() { return "can't open file"; }
	};
};

#endif    // WEBSERV_INCLUDES_CORE_CORE_EXCEPTION_H_
