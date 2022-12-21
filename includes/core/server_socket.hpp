#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <fcntl.h>

#include <cstdlib>
#include <iostream>

#include "socket.hpp" // core/socket.hpp
#include "server_info.hpp" // conf/server.hpp

class ServerSocket : public Socket {
   public:
	static const int BACK_LOG_QUEUE;
	explicit ServerSocket(const ServerInfo &s);
	~ServerSocket();

	void ListenSocket();
	int AcceptClient();

   private:
	void CreateSocket(const std::string &host, const std::string &port);
	void BindSocket(struct addrinfo *result);

};

#endif
