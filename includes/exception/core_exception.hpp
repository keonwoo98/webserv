//
// Created by 김민준 on 2022/12/20.
//

#ifndef WEBSERV_INCLUDES_CORE_CORE_EXCEPTION_HPP
#define WEBSERV_INCLUDES_CORE_CORE_EXCEPTION_HPP

#include <exception>

class CoreException {
   public:
	class ListenException : std::exception {
		const char *what() const throw() { return "listen() Failed"; }
	};

	class BindException : std::exception {
		const char *what() const throw() { return "bind() Failed"; }
	};

	class GetAddrInfoException : std::exception {
		const char *what() const throw() { return "getaddrinfo() Failed"; }
	};

	class KQueueException : std::exception {
		const char *what() const throw() { return "kqueue() failed"; }
	};
};

#endif	// WEBSERV_INCLUDES_CORE_CORE_EXCEPTION_H_
