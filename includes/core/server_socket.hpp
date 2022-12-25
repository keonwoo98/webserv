#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <fcntl.h>

#include <cstdlib>
#include <iostream>

#include "config_parser.hpp"
#include "server_info.hpp"	// conf/server.hpp
#include "socket.hpp"		// core/socket.hpp

class ServerSocket : public Socket {
   public:
	static const int BACK_LOG_QUEUE;
	explicit ServerSocket(const ConfigParser::server_config_type &server_config);
	~ServerSocket();

	const std::string &GetAddr() const;
	const std::vector<ServerInfo> &GetServerInfos() const;

	bool operator<(const ServerSocket &rhs) const;
	int AcceptClient();

   private:
	void CreateSocket();
	struct addrinfo *GetAddrInfos(const std::string &host,
								  const std::string &port);
	void Bind(struct addrinfo *result);
	void Listen();

	const std::string &addr_;
	const std::vector<ServerInfo> &server_infos_;
};

#endif
