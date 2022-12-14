#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <fcntl.h>

#include <cstdlib>
#include <iostream>

#include "socket.hpp"

class ServerSocket : public Socket {
   public:
	static const int BACK_LOG_QUEUE;
	ServerSocket(int host, int port);
	~ServerSocket();

	void ReadyToAccept();
	int AcceptClient();

   private:
	ServerSocket();
	void initSocket();
	void BindSocket();
	void ListenSocket();
	void SetSocketOption();
};

#endif
