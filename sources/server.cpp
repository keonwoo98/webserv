#include "server.hpp"
#include "socket.hpp"

#include <fcntl.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <vector>

Server::Server() { 
	kq_fd_ = kqueue();
	if (kq_fd_ < 0) {
		perror("kqueue error: ");
		// 예외처리
	}
}

Server::~Server() {}

void Server::Listen(Socket server_socket) {
	int server_sock_fd = server_socket.GetSocketFD();
	int state = listen(server_sock_fd, Server::BACK_LOG_QUEUE_SIZE);
	if (state < 0) {
		perror("listen error: ");
		// 예외처리
	}
}

void Server::ListenAddress(in_port_t port, in_addr_t ip_address) {
	Socket socket = Socket::CreateSocket(PF_INET, SOCK_STREAM, 0);
	int opt = 1; // ! ?
	socket.SetOption(SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	socket.BindAddress(AF_INET, port, ip_address);
	fcntl(socket.GetSocketFD(), F_SETFL, O_NONBLOCK);
	Listen(socket);
	listen_sockets_.insert(socket);
}

Socket Server::AcceptClient(Socket server_socket) {
	int server_sock_fd = server_socket.GetSocketFD();
	int client_sock_fd = accept(server_sock_fd, NULL, NULL);
	if (client_sock_fd < 0) {
		// error
	}
	fcntl(client_sock_fd, F_SETFL, O_NONBLOCK);
	std::cout << "client connected" << std::endl;
	return Socket(client_sock_fd);
}

struct kevent Server::CreateEvent(uintptr_t ident, int16_t filter, uint16_t flags,
						  uint32_t fflags, intptr_t data, void *udata) {
	struct kevent event;

	EV_SET(&event, ident, filter, flags, fflags, data, udata);
	return event;
}

void Server::run() {
	ListenAddress(8080, INADDR_ANY);
	ListenAddress(8081, INADDR_ANY);
	ListenAddress(8082, INADDR_ANY);

	const int EVENT_LIST_SIZE = 10;
	std::vector<struct kevent> change_list;
	struct kevent event_list[EVENT_LIST_SIZE];

	while (true) {
		int number_of_events = kevent(kq_fd_, change_list.data(), change_list.size(), event_list, EVENT_LIST_SIZE, NULL);
		change_list.clear();
		for (int i = 0; i < number_of_events; i++) {
			int fd = event_list[i].ident;

			if (event_list[i].flags & EV_EOF) {
				std::cout << "Disconnected" << std::endl;
				close(fd);
				continue;
			}
			if (event_list[i].filter == EVFILT_READ) {
				// READ EVENT
				std::set<Socket>::iterator it;
				if ((it = listen_sockets_.find(fd)) != listen_sockets_.end()) {
					Socket client = AcceptClient(*it);
					change_list.push_back(CreateEvent(client.GetSocketFD(), EVFILT_READ, EV_ADD, 0, 0, NULL));
				} else if ((it = clients_.find(fd)) != clients_.end()) {
					Socket client = *it;
					std::string message = client.readMessage();
					std::cout << message << std::endl;
					change_list.push_back(CreateEvent(client.GetSocketFD(), EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL));
				}
			} else if (event_list[i].filter == EVFILT_WRITE) {
				// WRITE_EVENT
				std::string response =
					"HTTP/1.1 404 Not Found\n"
					"Server: nginx/0.8.54\n"
					"Date: Mon, 02 Jan 2012 02:33:17 GMT\n"
					"Content-Type: text/html\n"
					"Content-Length: 147\n"
					"Connection: close";

				int count = send(fd, response.c_str(), response.length(), 0);
			}
		}
	}

}
