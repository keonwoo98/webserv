#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "server_info.hpp"
#include "socket.hpp"
#include "response_message.hpp"
#include "request_message.hpp"
#include "kqueue_handler.hpp"
#include "udata.h"

#define BUFFER_SIZE 1024

class ClientSocket : public Socket {
   public:
	explicit ClientSocket(int sock_d);
	ClientSocket(int sock_d, const std::vector<ServerInfo> &server_infos);
	~ClientSocket();
	void RecvRequest();
	void SendResponse(KqueueHandler &kqueue_handler, Udata *user_data);
	bool operator<(const ClientSocket &rhs) const;
};

bool ClientSocket::operator<(const ClientSocket &rhs) const {
	return sock_d_ < rhs.sock_d_;
}

#endif
