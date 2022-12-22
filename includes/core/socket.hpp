#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include "server_info.hpp"

class Socket {
   public:
	Socket(const std::vector<ServerInfo> &server_infos, int type);
	Socket(const std::vector<ServerInfo> &server_infos, int type, int sock_d);
	virtual ~Socket() = 0;

	enum {
		CLIENT_TYPE,
		SERVER_TYPE
	};

	const int &GetType() const;
	const int &GetSocketDescriptor() const;
	const std::vector<ServerInfo> &GetServerInfos() const;

	void Close() const;
   protected:
	const std::vector<ServerInfo> &server_infos_;
	int type_;
	int sock_d_;
	struct sockaddr_in address_;
};

std::ostream &operator<<(std::ostream &out, const Socket *socket);
#endif
