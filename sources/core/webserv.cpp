#include <sstream>

#include "webserv.hpp"
#include "event_executor.hpp"
#include "fd_handler.hpp"
#include "udata.hpp"
#include "unistd.h"

Webserv::Webserv(const server_configs_type &server_configs) {
	server_configs_type::const_iterator it;
	for (it = server_configs.begin(); it != server_configs.end(); ++it) {
		ServerSocket *server = new ServerSocket(it->second);
		int sock_d = server->GetSocketDescriptor();
		servers_.insert(std::make_pair(sock_d, server));

		Udata *user_data = new Udata(Udata::LISTEN, sock_d);
		kq_handler_.AddReadEvent(sock_d, user_data);
	}
}

Webserv::~Webserv() {}

void Webserv::RunServer() {
	while (true) {
		struct kevent event = kq_handler_.MonitorEvent();
		if (event.flags & EV_EOF) {
			close((int) event.ident);
			Udata *user_data = reinterpret_cast<Udata *>(event.udata);
			delete user_data;  // Socket is automatically removed from the kq
			continue;
		}
		HandleEvent(event);
	}
}

ServerSocket *Webserv::FindServerSocket(const int &fd) {
	return servers_.find(fd)->second;
}

ClientSocket *Webserv::FindClientSocket(const int &fd) {
	return clients_.find(fd)->second;
}

void Webserv::HandleEvent(struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	int event_fd = event.ident;
	int curr_state = user_data->GetState();
	int next_state;

	switch (curr_state) {
		case Udata::LISTEN:
			HandleListenEvent(FindServerSocket(event_fd));
			return;
		case Udata::RECV_REQUEST:
			next_state = HandleReceiveRequestEvent(FindClientSocket(event_fd),
												   user_data);
			break;
		case Udata::READ_FILE:
			next_state = HandleReadFile(event_fd, event.data, user_data);
			break;	// GET
		case Udata::WRITE_TO_PIPE:
			next_state = Udata::READ_FROM_PIPE;
			break;	// CGI
		case Udata::READ_FROM_PIPE:
			next_state = Udata::SEND_RESPONSE;
			break;	// CGI
		case Udata::SEND_RESPONSE:
			next_state =
				HandleSendResponseEvent(FindClientSocket(event_fd), user_data);
			break;
	}
	if (curr_state != next_state) {
		AddNextEvent(next_state, user_data);
	}
}

void Webserv::AddNextEvent(const int &next_state, Udata *user_data) {
	user_data->ChangeState(next_state);
	switch (next_state) {
		case Udata::READ_FILE:
			kq_handler_.AddReadEvent(OpenFile(*user_data), user_data);
			break;
		case Udata::WRITE_TO_PIPE:
			//  kq_handler_.AddWriteEvent(OpenPipe(client_socket, *user_data),
			//  user_data);
			break;
		case Udata::READ_FROM_PIPE:
			//  kq_handler_.AddReadEvent(OpenPipe(client_socket, *user_data),
			//  user_data);
			break;
		case Udata::SEND_RESPONSE:
			kq_handler_.AddWriteEvent(user_data->sock_d_, user_data);
			break;
		case Udata::RECV_REQUEST:
			user_data->Reset();
			kq_handler_.AddReadEvent(user_data->sock_d_, user_data);
			break;
		case Udata::CLOSE:
			delete user_data;
			break;
	}
}

void Webserv::HandleListenEvent(ServerSocket *server_socket) {
	int client_sock_d = EventExecutor::AcceptClient(server_socket);

	ClientSocket *client_socket = new ClientSocket(client_sock_d, server_socket->GetServerInfos());
	clients_.insert(std::make_pair(client_sock_d, client_socket));

	Udata *user_data = new Udata(Udata::RECV_REQUEST, client_sock_d);
	kq_handler_.AddReadEvent(client_sock_d, user_data);

	std::stringstream ss;
	ss << "new client accepted\n" << client_socket << std::endl;
}

int Webserv::HandleReceiveRequestEvent(ClientSocket *client_socket,
									   Udata *user_data) {
	int next_state;
	try {
		next_state = EventExecutor::ReceiveRequest(client_socket, user_data);
	} catch (std::exception &e) {
		next_state = Udata::SEND_RESPONSE;
		std::cerr << e.what() << std::endl;
	}
	if (next_state != Udata::RECV_REQUEST) {
		kq_handler_.DeleteReadEvent(user_data->sock_d_);
	}
	return next_state;
}

int Webserv::HandleReadFile(int fd, int readable_size, Udata *user_data) {
	int next_state;
	try {
		next_state = EventExecutor::ReadFile(fd, readable_size,
											 user_data->response_message_);
	} catch (const std::exception &e) {
		e.what();
	}
	return next_state;
}

int Webserv::HandleSendResponseEvent(ClientSocket *client_socket,
									 Udata *user_data) {
	int next_state;

	try {
		next_state = EventExecutor::SendResponse(client_socket->GetSocketDescriptor(), user_data);
	} catch (const std::exception &e) {
		e.what();
	}
	if (next_state != Udata::SEND_RESPONSE) {
		kq_handler_.DeleteWriteEvent(user_data->sock_d_);
	}
	return next_state;
}
