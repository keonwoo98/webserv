#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <ctime>

#include "socket.hpp"
#include "request_message.hpp"
#include "response_message.hpp"

class ClientSocket : public Socket {
   public:
	ClientSocket(int sock_d);
	~ClientSocket();

	const std::string GetRequest();
	int RecvRequest();
	void SendResponse();

   private:
	RequestMessage request_;
	ResponseMessage response_;

	std::string buffer_;

	ClientSocket();
	void CreateResponse();
};

#endif
