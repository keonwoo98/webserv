#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "socket.hpp"

class ClientSocket : public Socket {
   public:
	ClientSocket(int sock_d);
	~ClientSocket();

	const std::string &GetRequest() const;
	int RecvRequest();
	void SendResponse();

   private:
	ClientSocket();
	std::string request_;
	std::string response_;
};

#endif
