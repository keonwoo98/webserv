#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include "server.hpp"

class Socket {
   public:
	explicit Socket(const Server & s);
	virtual ~Socket() = 0;

	enum {
		CLIENT_TYPE,
		SERVER_TYPE
	};

	const int &GetType() const;
	const int &GetSocketDescriptor() const;
	const Server &GetServerInfo() const;

   protected:
	const Server & server_info_;
	int type_;
	int sock_d_;
	struct sockaddr_in address_;
};

std::ostream &operator<<(std::ostream &out, const Socket *socket);
#endif
