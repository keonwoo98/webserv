#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>
#include <sys/socket.h>

#include <iostream>
#include <vector>

#include "server_info.hpp"

class Socket {
   public:
	enum { CLIENT_TYPE, SERVER_TYPE };

	explicit Socket(int sock_d);
	virtual ~Socket() = 0;

	const int &GetSocketDescriptor() const;

	void Close() const;

   protected:
	int sock_d_;
	struct sockaddr_in address_;
};

std::ostream &operator<<(std::ostream &out, const Socket &socket);

#endif
