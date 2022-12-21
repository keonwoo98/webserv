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
    Webserv();
    ~Webserv();

	void AddServerKevent(ServerSocket *server);
	void AddClientKevent(ClientSocket *client);

	void DeleteClientPrevKevent(ClientSocket *client);

    void SetupServer(const ConfigParser::servers_type &servers);
    void StartServer();

private:
    KqueueHandler kq_handler_;

    void HandleServerSocketEvent(Udata *user_data);
    void HandleClientSocketEvent(Udata *user_data, struct kevent event);
};

#endif
