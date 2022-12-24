#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <vector>

#include "client_socket.hpp"
#include "kqueue_handler.hpp"
#include "server_socket.hpp"
#include "config_parser.hpp"
#include "udata.h"
#include "server_info.hpp"

class Webserv {
   public:
	explicit Webserv(const ConfigParser::use_type &use_map);
	~Webserv();
	void StartServer();
   private:
	void HandleEvent(struct kevent &event);
	void HandleListenEvent(const ServerSocket &server_socket);
	void HandleSendResponseEvent(ClientSocket client_socket, Udata *user_data);
	ServerSocket FindServerSocket(int fd);
	ClientSocket FindClientSocket(int fd);

	std::set<ClientSocket> clients_;
	std::set<ServerSocket> servers_;
	KqueueHandler kq_handler_;
};

#endif
