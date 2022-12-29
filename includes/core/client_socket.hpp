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

	ClientSocket(int sock_d, int server_d, const struct sockaddr_in &address);
	~ClientSocket();

	bool operator<(const ClientSocket &rhs) const;

	const ServerInfo &GetServerInfo() const;
	int GetServerFd() const;

	void FindServerInfoWithHost(const std::string &host);
	void FindLocationWithUri(const std::string &uri);

   private:
	// int sock_d_; 상속된 멤버
	// struct sockaddr_in address_; 상속된 멤버
	int server_d_;
	ServerInfo server_info_;
};

#endif
