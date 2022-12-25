#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <vector>

#include "client_socket.hpp"
#include "config_parser.hpp"
#include "kqueue_handler.hpp"
#include "server_info.hpp"
#include "server_socket.hpp"
#include "udata.h"

class Webserv {
   public:
	typedef typename ConfigParser::server_configs_type server_configs_type;
	typedef std::map<int, ServerSocket> servers_type;
	typedef std::map<int, ClientSocket> clients_type;

	explicit Webserv(const server_configs_type &server_configs);
	~Webserv();

	void StartServer();

   private:
	servers_type servers_;
	clients_type clients_;
	KqueueHandler kq_handler_;

	void HandleEvent(struct kevent &event);
	void HandleListenEvent(const ServerSocket &server_socket);
	void HandleSendResponseEvent(const ClientSocket &client_socket,
								 Udata *user_data);
	void HandleReceiveRequestEvent(ClientSocket &client_socket,
								   Udata *user_data);
	ServerSocket &FindServerSocket(const int &fd);
	ClientSocket &FindClientSocket(const int &fd);
};

#endif
