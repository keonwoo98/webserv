#include "webserv.hpp"

Webserv::Webserv() {}

Webserv::~Webserv() {}

void Webserv::SetupServer() {
    std::vector<int> port;
    port.push_back(8181);
    port.push_back(8282);
    port.push_back(8383);

	// collect kevents
	for (size_t i = 0; i < port.size(); ++i) {
		ServerSocket *server = new ServerSocket(0, port[i]);
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
