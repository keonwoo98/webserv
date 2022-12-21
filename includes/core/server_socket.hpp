#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <fcntl.h>

#include <cstdlib>
#include <iostream>

#include "socket.hpp"

class ServerSocket : public Socket {
   public:
	static const int BACK_LOG_QUEUE;
	ServerSocket(const std::string &host, const std::string &port);
	~ServerSocket();

	void ListenSocket();
	int AcceptClient();

   private:
	void CreateSocket(const std::string &host, const std::string &port);
	void BindSocket(struct addrinfo *result);

};

#endif
