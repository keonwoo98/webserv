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

class ClientSocket;
class ServerSocket;

class Webserv {
   public:
	typedef ConfigParser::server_configs_type server_configs_type;
	typedef std::map<int, ServerSocket *> servers_type;
	typedef std::map<int, ClientSocket *> clients_type;

	static int access_log_fd_;
	static int error_log_fd_;
	static servers_type servers_;
	static clients_type clients_;

	static ServerSocket *FindServerSocket(int fd);
	static ClientSocket *FindClientSocket(int fd);
	void RunServer();

	explicit Webserv(const server_configs_type &server_configs);
	~Webserv();
   private:
	KqueueHandler kq_handler_;

	void CreateListenSockets(const server_configs_type &server_confgis);

	void HandleEvent(struct kevent &event);
	void HandleListenEvent(struct kevent &event);
	void HandleReceiveRequestEvent(struct kevent &event);
	void HandleReadFile(struct kevent &event);
	void HandleWriteFile(struct kevent &event);
	void HandleSendResponseEvent(struct kevent &event);
	void HandleWriteToPipe(struct kevent &event);
	void HandleReadFromPipe(struct kevent &event);
	void HandleException(const HttpException &e, struct kevent &event);

	static void WriteLog(struct kevent &event);

	void DeleteClient(const struct kevent &event);
	void DeleteProcEvent(const struct kevent &event);

	bool IsProcessExit(const struct kevent &event) const;
	bool IsDisconnected(const struct kevent &event) const;
	bool IsLogEvent(const struct kevent &event) const;
};

#endif
