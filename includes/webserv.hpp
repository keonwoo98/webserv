#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <vector>
#include "client_socket.hpp"
#include "kqueue_handler.hpp"
#include "server_socket.hpp"

class Webserv {
   public:
	Webserv();
	~Webserv();

	void SetupServer();
	void StartServer();

   private:
	KqueueHandler kq_handler_;

	void HandleServerSocketEvent(Socket *socket);
	void HandleClientSocketEvent(Socket *socket);
};

#endif
