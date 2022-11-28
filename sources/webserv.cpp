#include "webserv.hpp"

#include <fstream>

#include "client_socket.hpp"
#include "server_socket.hpp"

Webserv::Webserv() {}

Webserv::~Webserv() {}

void Webserv::SetupServer() {
	std::vector<int> ports;	// host, ports Map 으로 변경하는 것은 어떤지
	ports.push_back(8081);
	ports.push_back(8181);
	ports.push_back(8282);

	// collect kevents
	for (size_t i = 0; i < ports.size(); ++i) {
		ServerSocket *server = new ServerSocket(INADDR_ANY, ports[i]);
		server->ReadyToAccept();
		struct kevent event = kq_handler_.CreateEvent(
			server->GetSocketDescriptor(), EVFILT_READ, EV_ADD, 0, 0, server);
		kq_handler_.addEventToChangeList(event);
	}
}

void Webserv::StartServer() {
	std::cout << "Start server" << std::endl;
	while (1) {
		// std::cout << "monitoring" << std::endl;
		const int EVENT_LIST_SIZE = 10;
		struct kevent event_list[EVENT_LIST_SIZE];

		int n_of_event = kq_handler_.MonitorEvents(event_list);
		for (int i = 0; i < n_of_event; ++i) {
			Socket *socket = reinterpret_cast<Socket *>(event_list[i].udata);

			if (event_list[i].flags & EV_EOF) {	 // Disconnected
				std::cout << "Disconnect" << std::endl;
				close(event_list[i].ident);
				delete socket;
				// Socket is automatically removed from the kq by the kernel
			} else if (event_list[i].flags & EV_ERROR) {
				std::cout << "kevent() Error" << std::endl;
				close(event_list[i].ident);
				delete socket;
			} else if (event_list[i].filter == EVFILT_READ) {	// Read Event
				if (socket->GetType() == Socket::SERVER_TYPE) {
					HandleServerSocketEvent(socket);
				} else if (socket->GetType() == Socket::CLIENT_TYPE) {
					HandleClientSocketEvent(socket);
				}
			} else if (event_list[i].filter == EVFILT_WRITE) {	// Write Event
				std::string response_header =
					"HTTP/1.1 404 Not Found\n"
					"Server: nginx/0.8.54\n"
					"Date: Mon, 02 Jan 2012 02:33:17 GMT\n"
					"Content-Type: text/html\n"
					"Content-Length: 147\n"
					"Connection: close";
				send(socket->GetSocketDescriptor(), response.c_str(), response.length(), 0);
			}
		}
	}
}

void Webserv::HandleServerSocketEvent(Socket *socket) {
	ServerSocket *server = dynamic_cast<ServerSocket *>(socket);
	ClientSocket *client = new ClientSocket(server->AcceptClient());

	struct kevent event = kq_handler_.CreateEvent(
		client->GetSocketDescriptor(), EVFILT_READ, EV_ADD, 0, 0, client);
	kq_handler_.addEventToChangeList(event);
	std::cout << "Got connection\n" << client << std::endl;
}

void Webserv::HandleClientSocketEvent(Socket *socket) {
	ClientSocket *client = dynamic_cast<ClientSocket *>(socket);
	std::cout << "read event from:\n" << client << std::endl;
	struct kevent event =
		kq_handler_.CreateEvent(client->GetSocketDescriptor(), EVFILT_WRITE,
							  EV_ADD | EV_ONESHOT, 0, 0, client);
	kq_handler_.addEventToChangeList(event);
	int n = client->ReadMessage();
	if (n >= 0) {
		std::cout << "Message : " << client->GetMessage();
	}
}
