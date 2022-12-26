#include "webserv.hpp"

#include "event_handler.hpp"
#include "udata.h"
#include "fd_handler.h"
#include "unistd.h"

Webserv::Webserv(const server_configs_type &server_configs) {
	for (server_configs_type::const_iterator it = server_configs.begin();
			it != server_configs.end(); ++it) {
		ServerSocket server(it->first, it->second);
		servers_.insert(std::make_pair(server.GetSocketDescriptor(), server));
		kq_handler_.AddReadEvent(
				server.GetSocketDescriptor(),
				reinterpret_cast<void *>(new Udata(Udata::LISTEN, server.GetSocketDescriptor())));  // LISTEN 이벤트 등록
	}
}

Webserv::~Webserv() {}

// TODO : ISSUE에 이름 바꾸자고 건의하기
void Webserv::StartServer() {
	std::cout << "Start server" << std::endl;
	while (true) {
		struct kevent event =
				kq_handler_.MonitorEvent();     // 이벤트 등록 & 이벤트 추출

		if (event.flags & EV_EOF) {
			std::cout << "Disconnect : " << event.ident << std::endl;
			close(event.ident);
			Udata *user_data = reinterpret_cast<Udata *>(event.udata);
			delete user_data;  // Socket is automatically removed from the kq by
			// the kernel
			continue;
		}
		HandleEvent(event);
	}
}

ServerSocket &Webserv::FindServerSocket(const int &fd) {
    return servers_.find(fd)->second;
}

ClientSocket &Webserv::FindClientSocket(const int &fd) {
    return clients_.find(fd)->second;
}

void Webserv::HandleEvent(struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	int event_fd = event.ident;
	switch (user_data->GetState()) {
		case Udata::LISTEN:
			HandleListenEvent(FindServerSocket(event_fd));
			break;
		case Udata::RECV_REQUEST:
			HandleReceiveRequestEvent(FindClientSocket(event_fd), user_data);
			break;
		case Udata::READ_FILE:
			HandleReadFile(user_data, event_fd);
			break;    // GET
		case Udata::WRITE_TO_PIPE:
			break;    // CGI
		case Udata::READ_FROM_PIPE:
			break;    // CGI
		case Udata::SEND_RESPONSE:
			HandleSendResponseEvent(FindClientSocket(event_fd), user_data);
			break;
	}
}

void Webserv::HandleListenEvent(const ServerSocket &server_socket) {
	int client_sock_d = EventHandler::HandleListenEvent(server_socket);

	ClientSocket client_socket(client_sock_d, server_socket.GetServerInfos());
	clients_.insert(std::make_pair(client_sock_d, client_socket));

	Udata *udata = new Udata(Udata::RECV_REQUEST, client_sock_d);
	kq_handler_.AddReadEvent(client_sock_d, reinterpret_cast<void *>(udata));
	std::cout << "new client" << '\n' << client_socket << std::endl;
}

void Webserv::HandleReceiveRequestEvent(ClientSocket &client_socket,
										Udata *user_data) {
	try {
		int next_state = EventHandler::HandleRequestEvent(client_socket, user_data);
		if (next_state == Udata::RECV_REQUEST) {
			return;
		}
		kq_handler_.DeleteReadEvent(client_socket.GetSocketDescriptor());
		user_data->ChangeState(next_state);
		if (next_state == Udata::READ_FILE) {
			kq_handler_.AddReadEvent(OpenFile(*user_data), user_data);
		} else if (next_state == Udata::READ_FROM_PIPE) {
			//  kq_handler_.AddReadEvent(OpenPipe(client_socket, *user_data), user_data);
		} else if (next_state == Udata::WRITE_TO_PIPE) {
			//  kq_handler_.AddWriteEvent(OpenPipe(client_socket, *user_data), user_data);
		} else if (next_state == Udata::SEND_RESPONSE) {
			kq_handler_.AddWriteEvent(client_socket.GetSocketDescriptor(), user_data);
		}
	} catch (std::exception &e) {
		kq_handler_.DeleteReadEvent(client_socket.GetSocketDescriptor());
		std::cerr << e.what() << std::endl;
		user_data->ChangeState(Udata::SEND_RESPONSE);
		kq_handler_.AddWriteEvent(client_socket.GetSocketDescriptor(), user_data);
	}

}

void Webserv::HandleReadFile(Udata *user_data, int fd) {
	// Get Request만 들어왔을때 가정
	// 저장해주는 객체 필요함 (udata에 저장) 만들 예정
	char buf[BUFFER_SIZE + 1];
	int size = read(fd, buf, BUFFER_SIZE);
	if (size < 0) {
		std::perror("open: INTERNAL_SERVER_ERROR");
		// return INTERNAL_SERVER_ERROR;
	}
	user_data->response_message_.SetBody(buf);
	// buf 이하로 읽고 and 성공했다는 가정
	// test 용도임
	kq_handler_.DeleteWriteEvent(fd);
	// 여기서 response 설정을 해줘야함
	user_data->ChangeState(Udata::SEND_RESPONSE);
	kq_handler_.AddWriteEvent(user_data->sock_d_, user_data);
}

void Webserv::HandleSendResponseEvent(const ClientSocket &client_socket,
										Udata *user_data) {
	int result = EventHandler::HandleResponseEvent(client_socket, user_data);
	if (result == EventHandler::KEEP_ALIVE) {
		kq_handler_.DeleteWriteEvent(
				client_socket.GetSocketDescriptor());  // event 삭제
		user_data->Reset(); // change state도 함께 일어남
		kq_handler_.AddReadEvent(client_socket.GetSocketDescriptor(),
									user_data);  // read event 등록
	} else if (result == EventHandler::CLOSE) {
		// clients에서 client 소켓 삭제
		delete user_data;
	} else if (result == EventHandler::ERROR) {
	}
	// HAS_MORE
}
