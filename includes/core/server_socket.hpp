#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <fcntl.h>

#include <cstdlib>
#include <iostream>

#include "config_parser.hpp"
#include "server_info.hpp"	// conf/server.hpp
#include "socket.hpp"		// core/socket.hpp
#include "client_socket.hpp"
#include "webserv.hpp"		// core/webserv.hpp

class ClientSocket;

class ServerSocket : public Socket {
   public:
	static const int BACK_LOG_QUEUE;
	typedef ConfigParser::server_infos_type server_infos_type;

	explicit ServerSocket(const server_infos_type &server_infos);
	~ServerSocket();

	const server_infos_type &GetServerInfos() const;

	bool operator<(const ServerSocket &rhs) const;
	ClientSocket *AcceptClient();

   private:
	const server_infos_type &server_infos_;
	void CreateSocket(const std::string &host, const std::string &port);
	struct addrinfo *GetAddrInfos(const std::string &host,
								  const std::string &port);
	void Bind(struct addrinfo *result);
	void Listen();
};

#endif
