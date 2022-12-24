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
	ServerSocket();
	ServerSocket(const std::string &host, const std::string &port);
	~ServerSocket();
	bool operator<(const ServerSocket &rhs) const;

	struct addrinfo *GetAddrInfo(const std::string &host, const std::string &port);
	int AcceptClient();
   private:
	void CreateSocket(const std::string &host, const std::string &port);
	void Bind(struct addrinfo *result);
	void Listen();
};

bool ServerSocket::operator<(const ServerSocket &rhs) const {
	return sock_d_ < rhs.sock_d_;
}

#endif
