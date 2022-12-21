#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "server.hpp"
#include "socket.hpp"
#include "response_message.hpp"
#include "request_message.hpp"

class ClientSocket : public Socket {
   public:
	ClientSocket(int sock_d, const Server &server_info);
	~ClientSocket();
	void RecvRequest();
	void SendResponse();
   private:
	RequestMessage request_;
	ResponseMessage response_;
};

#endif
