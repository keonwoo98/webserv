#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "client_socket.hpp"
#include "config_parser.hpp"
#include "kqueue_handler.hpp"
#include "server_info.hpp"
#include "server_socket.hpp"

class ServerSocket;
class ClientSocket;

class Webserv {
   public:
	typedef ConfigParser::server_configs_type server_configs_type;
	typedef std::map<int, ServerSocket *> servers_type;
	typedef std::map<int, ClientSocket *> clients_type;

	/** Log File Descriptors */
	static int access_log_fd_;
	static int error_log_fd_;
	/** sockets */
	static servers_type servers_;
	static clients_type clients_;
	/** socket utils */
	static ServerSocket *FindServerSocket(int fd);
	static ClientSocket *FindClientSocket(int fd);

	explicit Webserv(const server_configs_type &server_configs);
	~Webserv();

	void RunServer();
   private:
	KqueueHandler kq_handler_;

	void CreateListenSockets(const server_configs_type &server_confgis);
	void HandleEvent(struct kevent &event);
	void HandleCgi(struct kevent &event);
	void HandleException(const HttpException &e, struct kevent &event);
	static void WriteLog(struct kevent &event);
	void DeleteClient(const struct kevent &event);
	void DeleteProcEvent(const struct kevent &event);
};

#endif
