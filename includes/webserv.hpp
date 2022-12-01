#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <vector>

#include "kqueue_handler.hpp"
#include "socket.hpp"

class Webserv {
   public:
	Webserv();
	~Webserv();

	void SetupServer();
	void StartServer();

   private:
	KqueueHandler kq_handler_;

	void HandleServerSocketEvent(Socket *socket);
	void HandleClientSocketEvent(Socket *socket, struct kevent event);
};

#endif
