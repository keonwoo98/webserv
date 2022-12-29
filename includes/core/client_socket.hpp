#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "kqueue_handler.hpp"
#include "request_message.hpp"
#include "response_message.hpp"
#include "server_info.hpp"
#include "server_socket.hpp"
#include "socket.hpp"
#include "udata.hpp"

class ClientSocket : public Socket {
   public:

	ClientSocket(int sock_d, const struct sockaddr_in &address);
	~ClientSocket();

	bool operator<(const ClientSocket &rhs) const;

	const ServerInfo &GetServerInfo() const;

	void FindServerInfoWithHost(const std::string &host);
	void FindLocationWithUri(const std::string &uri);

   private:
	// int sock_d_;
	// struct sockaddr_in address_;
	int server_fd_;
	ServerInfo server_info_;
};

#endif
