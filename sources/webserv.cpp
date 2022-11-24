#include "webserv.hpp"

Webserv::Webserv() {}

Webserv::~Webserv() {}

void Webserv::SetupServer() {
	std::vector<int> port;
	port.push_back(8080);
	port.push_back(8181);
	port.push_back(8282);

	// collect kevents
	for (int i = 0; i < port.size(); ++i) {
		ServerSocket *server = new ServerSocket(0, port[i]);
		server->ReadyToAccept();
		kq_handler_.CollectEvents(server->GetSocketDescriptor(), EVFILT_READ,
								 EV_ADD, 0, 0, server);
	}
}

void Webserv::StartServer() {
	std::cout << "Start server" << std::endl;
	while (1) {
		// std::cout << "monitoring" << std::endl;
		std::vector<struct kevent> event_list;
		event_list = kq_handler_.MonitorEvents();
		for (int i = 0; i < event_list.size(); ++i) {
			Socket *socket = reinterpret_cast<Socket *>(event_list[i].udata);
			if (event_list[i].flags & EV_EOF) {
				std::cout << "Disconnect" << std::endl;
				close(event_list[i].ident);
				// Socket is automatically removed from the kq by the kernel
			} else if (event_list[i].filter == EVFILT_READ) {
				if (socket->GetType() == Socket::SERVER_TYPE) {
					HandleServerSocketEvent(socket);
				} else if (socket->GetType() == Socket::CLIENT_TYPE) {
					HandleClientSocketEvent(socket);
				}
			} else if (event_list[i].filter == EVFILT_WRITE) {
				write(socket->GetSocketDescriptor(), "hello\n", 6);
			}
		}
	}
}

void Webserv::HandleServerSocketEvent(Socket *socket) {
	ServerSocket *server = dynamic_cast<ServerSocket *>(socket);
	ClientSocket *client = new ClientSocket(server->AcceptClient());
	kq_handler_.CollectEvents(client->GetSocketDescriptor(), EVFILT_READ,
							  EV_ADD, 0, 0, client);
	std::cout << "Got connection " << client->GetSocketDescriptor()
			  << std::endl;
}

void Webserv::HandleClientSocketEvent(Socket *socket) {
	ClientSocket *client = dynamic_cast<ClientSocket *>(socket);
	client->ReadMessage();
	std::cout << "Get message from " << client->GetSocketDescriptor()
			  << std::endl;
	kq_handler_.CollectEvents(client->GetSocketDescriptor(),
							  EVFILT_WRITE | EV_ONESHOT, EV_ADD, 0, 0, client);
	std::cout << client->GetMessage();
}
