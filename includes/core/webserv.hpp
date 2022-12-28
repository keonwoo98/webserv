#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <vector>
#include <fstream>

#include "client_socket.hpp"
#include "config_parser.hpp"
#include "kqueue_handler.hpp"
#include "server_info.hpp"
#include "server_socket.hpp"
#include "udata.hpp"

class Webserv {
   public:
	typedef ConfigParser::server_configs_type server_configs_type;
	typedef std::map<int, ServerSocket *> servers_type;
	typedef std::map<int, ClientSocket *> clients_type;

	static unsigned long error_log_fd_;
	static unsigned long access_log_fd_;

	explicit Webserv(const server_configs_type &server_configs);
	~Webserv();

	void RunServer();
   private:
	servers_type servers_;
	clients_type clients_;
	KqueueHandler kq_handler_;

	void HandleEvent(struct kevent &event);
	void AddNextEvent(const int &next_state, Udata *user_data);

	void HandleListenEvent(ServerSocket *server_socket);
	int HandleReceiveRequestEvent(ClientSocket *client_socket, Udata *user_data);
	int HandleReadFile(int fd, int readable_size, Udata *user_data);
	int HandleSendResponseEvent(ClientSocket *client_socket, Udata *user_data);

	static void WriteLog(struct kevent &event);

	ServerSocket *FindServerSocket(const int &fd);
	ClientSocket *FindClientSocket(const int &fd);
};

#endif
