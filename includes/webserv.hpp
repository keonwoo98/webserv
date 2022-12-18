#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <vector>
#include "client_socket.hpp"
#include "kqueue_handler.hpp"
#include "server_socket.hpp"
#include "config_parser.hpp"
#include "server.hpp"

class Webserv {
public:
    Webserv();
    ~Webserv();

	void AddServerKevent(ServerSocket *server);
	void AddClientKevent(ClientSocket *client);

	void DeleteClientKevent(ClientSocket *client);

    void SetupServer(const ConfigParser::servers_type &servers);
    void StartServer();

private:
    KqueueHandler kq_handler_;

    void HandleServerSocketEvent(Socket *socket);
    void HandleClientSocketEvent(Socket *socket, struct kevent event);
};

#endif
