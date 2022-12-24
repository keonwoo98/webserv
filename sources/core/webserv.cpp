#include "webserv.hpp"
#include "udata.h"
#include "event_handler.hpp"

Webserv::Webserv(const ConfigParser::use_type &use_map) {
	ConfigParser::use_type::const_iterator it;

	for (it = use_map.begin(); it != use_map.end(); ++it) {
		ServerSocket server(it->second);
		servers_.insert(server);
		kq_handler_.AddReadEvent(server.GetSocketDescriptor(),
								 reinterpret_cast<void *>(new Udata(LISTEN))); // LISTEN 이벤트 등록
	}
}

Webserv::~Webserv() {}

void Webserv::StartServer() {
	std::cout << "Start server" << std::endl;
	while (true) {
		struct kevent event = kq_handler_.MonitorEvent(); // 이벤트 등록 & 이벤트 추출

		if (event.flags & EV_EOF) {
			std::cout << "Disconnect : " << event.ident << std::endl;
			Udata *user_data = reinterpret_cast<Udata *>(event.udata);
			delete user_data; // Socket is automatically removed from the kq by the kernel
			continue;
		}
		HandleEvent(event);
	}
}

ServerSocket Webserv::FindServerSocket(int fd) {
	ServerSocket server_socket(fd);
	servers_.find(fd);
}

ClientSocket Webserv::FindClientSocket(int fd) {
	for (size_t i = 0; i < clients_.size(); ++i) {
		if (clients_[i].GetSocketDescriptor() == fd) {
			return clients_[i];
		}
	}
}

void Webserv::HandleEvent(struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	int event_fd = event.ident;
	switch (user_data->type_) {
		case LISTEN: HandleListenEvent(FindServerSocket(event_fd));
			break;
		case RECV_REQUEST:
			break;
		case SEND_RESPONSE: HandleSendResponseEvent(FindClientSocket(event_fd), user_data);
			break;
		case READ_FILE: break; // GET
		case PIPE_READ: break; // CGI
		case PIPE_WRITE: break; // CGI
	}
}

void Webserv::HandleListenEvent(const ServerSocket &server_socket) {
	int client_sock_d = EventHandler::HandleListenEvent(server_socket);

	ClientSocket client_socket(client_sock_d, server_socket.GetServerInfos());
	clients_.push_back(client_socket);

	Udata *udata = new Udata(RECV_REQUEST);
	kq_handler_.AddReadEvent(client_socket.GetSocketDescriptor(), reinterpret_cast<void *>(udata));
	std::cout << "new client" << '\n' << client_socket << std::endl;
}

void Webserv::HandleSendResponseEvent(ClientSocket client_socket, Udata *user_data) {
	int result = EventHandler::HandleResponseEvent(client_socket, user_data);
	if (result == EventHandler::KEEP_ALIVE) {
		kq_handler_.DeleteWriteEvent(client_socket.GetSocketDescriptor(), NULL); // event 삭제
		user_data->Reset();
		kq_handler_.AddReadEvent(client_socket.GetSocketDescriptor(), user_data); // read event 등록
	} else if (result == EventHandler::CLOSE) {
		// clients에서 client 소켓 삭제
		delete user_data;
	} else if (result == EventHandler::ERROR) {

	}
	// HAS_MORE
}
