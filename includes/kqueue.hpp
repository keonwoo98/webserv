#ifndef KQUEUE_HPP
#define KQUEUE_HPP

#include "server_socket.hpp"

class Kqueue {
   private:
	int _kqueue;
	ServerSocket _socket;
	std::map<int, std::string> _clients;
	std::vector<struct kevent> _change_list;

   public:
	Kqueue();
	void CreateKqueue();
	void RegisterKevent(uintptr_t fd);
	void MonitorKevent();

	class KqueueException : public std::exception {
	   public:
		const char *what() const throw() { return "Error: kqueue failed"; }
	};

	class KeventException : public std::exception {
	   public:
		const char *what() const throw() { return "Error: kevent failed"; }
	};

	class AcceptException : public std::exception {
	   public:
		const char *what() const throw() { return "Error: accept failed"; }
	};

	class ReadException : public std::exception {
	   public:
		const char *what() const throw() { return "Error: read failed"; }
	};
};

#endif
