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

	static std::string HostToIpAddr(uint32_t addr);
	static int GetSendBufferSize(int fd);

	explicit Socket(int sock_d);
	Socket(int sock_d, const struct sockaddr_in &address);
	virtual ~Socket() = 0;

	const int &GetSocketDescriptor() const;
	std::string GetAddr() const;
	std::string GetPort() const;

	static std::string GetAddr(const int &fd);
	static std::string GetPort(const int &fd);

	std::string ToString() const;
	void Close() const;
   protected:
	int sock_d_;
	struct sockaddr_in address_;
};

std::ostream &operator<<(std::ostream &out, const Socket *socket);

#endif
