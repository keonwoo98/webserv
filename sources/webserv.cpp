#include "webserv.hpp"

#include <fstream>

#include "client_socket.hpp"
#include "server_socket.hpp"
#include "request/request_message.hpp"

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
		AddServerKevent(server);
	}
}

void Webserv::AddServerKevent(ServerSocket *server) {
	kq_handler_.CollectEvents(server->GetSocketDescriptor(), EVFILT_READ,
							  EV_ADD, 0, 0, server);
}

void Webserv::AddClientKevent(ClientSocket *client) {
	if (client->GetPrevState() != client->GetState()) {
		DeleteClientKevent(client);
	} else {
		return;
	}

	switch (client->GetState()) {
		case ClientSocket::REQUEST:
			kq_handler_.CollectEvents(client->GetSocketDescriptor(),
									  EVFILT_READ, EV_ADD, 0, 0, client);
			break;
		case ClientSocket::READ_FILE:
			break;
		case ClientSocket::READ_CGI:
			break;
		case ClientSocket::RESPONSE:
			kq_handler_.CollectEvents(client->GetFileDescriptor(), EVFILT_READ,
									  EV_ADD, 0, 0, client);
			break;
		case ClientSocket::WRITE_FILE:
			break;
		case ClientSocket::WRITE_CGI:
			break;
		case ClientSocket::DONE:
			break;
	}
}

void Webserv::DeleteClientKevent(ClientSocket *client) {
	switch (client->GetState()) {
		case ClientSocket::REQUEST:
			kq_handler_.CollectEvents(client->GetSocketDescriptor(),
									  EVFILT_READ, EV_DELETE, 0, 0, client);
			break;
		case ClientSocket::READ_FILE:
			break;
		case ClientSocket::READ_CGI:
			break;
		case ClientSocket::RESPONSE:
			kq_handler_.CollectEvents(client->GetFileDescriptor(), EVFILT_READ,
									  EV_DELETE, 0, 0, client);
			break;
		case ClientSocket::WRITE_FILE:
			break;
		case ClientSocket::WRITE_CGI:
			break;
		case ClientSocket::DONE:
			break;
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
				std::cout << "system error : " << event_list[i].fflags << std::endl;
				close(event_list[i].ident);
				delete socket;
				// Socket is automatically removed from the kq by the kernel
			} else {
				if (socket->GetType() == Socket::SERVER_TYPE) {
					// std::cout << "size of listen backlog: " << event_list[i].data << std::endl;
					HandleServerSocketEvent(socket);
				} else {
					HandleClientSocketEvent(socket, event_list[i]);
				}
			}
		}
	}
}

void Webserv::HandleClientSocketEvent(Socket *socket, struct kevent event) {
	ClientSocket *client = dynamic_cast<ClientSocket *>(socket);

	(void)event;
	switch (client->GetState()) {
		case ClientSocket::REQUEST:
			client->RecvRequest();
			AddClientKevent(client);
			break;
		case ClientSocket::READ_FILE:
			break;
		case ClientSocket::READ_CGI:
			break;
		case ClientSocket::RESPONSE:
			client->PrintRequest();
			client->SendResponse();
			client->ResetParsingState();
			AddClientKevent(client);
			break;
		case ClientSocket::WRITE_FILE:
			break;
		case ClientSocket::WRITE_CGI:
			break;
		case ClientSocket::DONE:
			break;
	}
}

void Webserv::HandleServerSocketEvent(Socket *socket) {
	ServerSocket *server = dynamic_cast<ServerSocket *>(socket);
	ClientSocket *client = new ClientSocket(server->AcceptClient());
	AddClientKevent(client);
	std::cout << "Got connection " << client->GetSocketDescriptor()
			  << std::endl;
}
