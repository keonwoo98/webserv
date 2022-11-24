#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <fcntl.h>

#include <cstdlib>
#include <iostream>

#include "socket.hpp"

class ServerSocket : public Socket {
   public:
	ServerSocket(int host, int port);
	~ServerSocket();

	void ReadyToAccept();
	int AcceptClient();

   private:
	ServerSocket();
	void CreateSocket();
	void BindSocket();
	void ListenSocket();
};

#endif
