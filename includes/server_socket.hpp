#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <set>
#include <vector>

class ServerSocket {
   private:
	std::set<int> _listening_sockets;

   public:
	ServerSocket();
	int CreateSocket(int port);
	int FindFd(int fd);

	class SocketException : public std::exception {
	   public:
		const char *what() const throw() {
			return "Error: socket failed";
		}
	};

	class SetsockoptException : public std::exception {
	   public:
		const char *what() const throw() {
			return "Error: setsockopt failed";
		}
	};

	class BindException : public std::exception {
	   public:
		const char *what() const throw() {
			return "Error: bind failed";
		}
	};

	class ListenException : public std::exception {
	   public:
		const char *what() const throw() {
			return "Error: listen failed";
		}
	};

	class FcntlException : public std::exception {
	   public:
		const char *what() const throw() {
			return "Error: fcntl failed";
		}
	};
};

#endif
