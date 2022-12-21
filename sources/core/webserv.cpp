#include "webserv.hpp"
#include "udata.h"

#include <unistd.h>

Webserv::Webserv() {}

Webserv::~Webserv() {}

void Webserv::SetupServer(const ConfigParser::servers_type &servers) {
	// ConfigParser::servers_type은 현재 vector인데, map으로 변경 될 예정

	for (size_t i = 0; i < servers.size(); ++i) {
		const ServerInfo &server_info = servers[i];
		ServerSocket *server = new ServerSocket(server_info); // Server Socket 생성
		Udata *udata = new Udata(LISTEN, server);
		kq_handler_.AddReadEvent(server->GetSocketDescriptor(), reinterpret_cast<void *>(udata)); // LISTEN 이벤트 등록
	}
}

void Webserv::StartServer() {
	std::cout << "Start server" << std::endl;
	while (true) {
		// std::cout << "monitoring" << std::endl;
		std::vector<struct kevent> event_list;
		event_list = kq_handler_.MonitorEvents();
		for (size_t i = 0; i < event_list.size(); ++i) {
			Udata *user_data = reinterpret_cast<Udata *>(event_list[i].udata);
			if (event_list[i].flags & EV_EOF) {
				std::cout << "Disconnect" << std::endl;
				close(event_list[i].ident);
				delete user_data;
				// Socket is automatically removed from the kq by the kernel
			} else {
				if (user_data->type_ == LISTEN) {
					HandleServerSocketEvent(user_data);
				} else {
					HandleClientSocketEvent(user_data, event_list[i]);
				}
			}
		}
	}
}

void Webserv::HandleServerSocketEvent(Udata *udata) {
	ServerSocket *server_socket = reinterpret_cast<ServerSocket *>(udata->socket_);
	int client_sock_d = server_socket->AcceptClient();
	ClientSocket *client = new ClientSocket(client_sock_d, server_socket->GetServerInfo());
	AddClientKevent(client);
	std::cout << "Got connection " << client->GetSocketDescriptor()
			  << std::endl;
}

void Webserv::AddClientKevent(ClientSocket *client) {
	Udata *udata = new Udata(RECV_REQUEST, client);
	kq_handler_.AddReadEvent(client->GetSocketDescriptor(), reinterpret_cast<void *>(udata));
}

void Webserv::HandleClientSocketEvent(Udata *user_data, struct kevent event) {
//	ClientSocket *client = dynamic_cast<ClientSocket *>(user_data->socket_);
	(void)event;
	switch (user_data->type_) {
		case RECV_REQUEST: break;
		case SEND_RESPONSE: break;
		case READ_FILE: break; // GET
		case PIPE_READ: break; // CGI
		case PIPE_WRITE: break; // CGI
	}
}
