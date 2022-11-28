#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <vector>

#include "socket.hpp"

class ClientSocket : public Socket {
   public:
	static const int BUFFER_SIZE;
	ClientSocket(int sock_d);
	~ClientSocket();
	const std::string &GetMessage() const;
	int ReadMessage();

   private:
	ClientSocket();
	std::string message_;
};

#endif
