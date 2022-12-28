#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>
#include <sys/socket.h>

#include <iostream>
#include <vector>

#include "config_parser.hpp"
#include "server_info.hpp"

class Socket {
   public:
	typedef ConfigParser::server_infos_type server_infos_type;

	Socket(int sock_d, const server_infos_type &server_infos);
	virtual ~Socket() = 0;

	const int &GetSocketDescriptor() const;

	void Close() const;

   protected:
	const server_infos_type &server_infos_;
	int sock_d_;
	struct sockaddr_in address_;
};

std::ostream &operator<<(std::ostream &out, const Socket &socket);

#endif
