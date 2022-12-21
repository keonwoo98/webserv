#include "webserv.hpp"

Webserv::Webserv() {}

Webserv::~Webserv() {}

void Webserv::SetupServer(const ConfigParser::servers_type &servers) {
	std::vector<std::string> port;
	port.push_back("8181");
	port.push_back("8282");
	port.push_back("8383");

	// collect kevents
	for (size_t i = 0; i < port.size(); ++i) {
		ServerSocket *server = new ServerSocket("localhost", port[i]);
		server->ListenSocket();
		AddServerKevent(server);
	}

	for (size_t i = 0; i < servers.size(); ++i) {
		Server item = servers[i];
		ServerSocket *server = new ServerSocket(item.GetHost(), item.GetPort());
		AddServerKevent(server);
	}
}

void Webserv::AddServerKevent(ServerSocket *server) {
	kq_handler_.AddReadEvent(server->GetSocketDescriptor(), server);
}

void Webserv::DeleteClientPrevKevent(ClientSocket *client) {
	switch (client->GetPrevState()) {
		case ClientSocket::INIT:
			break;
		case ClientSocket::REQUEST:
			kq_handler_.DeleteReadEvent(client->GetSocketDescriptor(), client);
			break;
		case ClientSocket::READ_FILE:
			kq_handler_.DeleteReadEvent(client->GetFileDescriptor(), client);
			break;
		case ClientSocket::READ_CGI:
			break;
		case ClientSocket::WRITE_FILE:
			break;
		case ClientSocket::WRITE_CGI:
			break;
		case ClientSocket::RESPONSE:
			kq_handler_.DeleteWriteEvent(client->GetSocketDescriptor(), client);
			break;
	}
}

void Webserv::AddClientKevent(ClientSocket *client) {
	if (!client->IsStateChanged()) {
		return;
	}
	DeleteClientPrevKevent(client);
	switch (client->GetState()) {
		case ClientSocket::INIT:
			break;
		case ClientSocket::REQUEST:
			kq_handler_.AddReadEvent(client->GetSocketDescriptor(), client);
			break;
		case ClientSocket::READ_FILE:
			kq_handler_.AddReadEvent(client->GetFileDescriptor(), client);
			break;
		case ClientSocket::READ_CGI:
			break;
		case ClientSocket::WRITE_FILE:
			break;
		case ClientSocket::WRITE_CGI:
			break;
		case ClientSocket::RESPONSE:
			kq_handler_.AddWriteEvent(client->GetSocketDescriptor(), client);
			break;
	}
}

void Webserv::StartServer() {
	std::cout << "Start server" << std::endl;
	while (1) {
		// std::cout << "monitoring" << std::endl;
		std::vector<struct kevent> event_list;
		event_list = kq_handler_.MonitorEvents();
		for (size_t i = 0; i < event_list.size(); ++i) {
			Socket *socket = reinterpret_cast<Socket *>(event_list[i].udata);
			if (event_list[i].flags & EV_EOF) {
				std::cout << "Disconnect" << std::endl;
				close(event_list[i].ident);
				// Socket is automatically removed from the kq by the kernel
			} else {
				if (socket->GetType() == Socket::SERVER_TYPE) {
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
		case ClientSocket::INIT:
			break;
		case ClientSocket::REQUEST:
			client->RecvRequest();
			break;
		case ClientSocket::READ_FILE:
			client->ReadFile(event.data);
			break;
		case ClientSocket::READ_CGI:
			break;
		case ClientSocket::RESPONSE:
			client->SendResponse();
			break;
		case ClientSocket::WRITE_FILE:
			break;
		case ClientSocket::WRITE_CGI:
			break;
	}
	AddClientKevent(client);
}

void Webserv::HandleServerSocketEvent(Socket *socket) {
	ServerSocket *server = dynamic_cast<ServerSocket *>(socket);
	ClientSocket *client = new ClientSocket(server->AcceptClient());
	AddClientKevent(client);
	std::cout << "Got connection " << client->GetSocketDescriptor()
			  << std::endl;
}
