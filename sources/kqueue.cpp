#include "kqueue.hpp"

Kqueue::Kqueue() { CreateKqueue(); }

void Kqueue::CreateKqueue() {
	_kqueue = kqueue();
	if (_kqueue == -1) throw KqueueException();
}

void Kqueue::RegisterKevent(uintptr_t fd) {
	struct kevent event;

	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	_change_list.push_back(event);
}

void Kqueue::MonitorKevent() {
	int n_of_events;
	struct kevent event_list[8];
	RegisterKevent(_socket.CreateSocket(8080));
	RegisterKevent(_socket.CreateSocket(8181));
	RegisterKevent(_socket.CreateSocket(8282));

	std::cout << "server start" << std::endl;
	while (true) {
		n_of_events = kevent(_kqueue, (struct kevent *)_change_list.data(),
							 _change_list.size(), event_list, 8, NULL);
		if (n_of_events == -1) throw KeventException();
		_change_list.clear();
		for (int i = 0; i < n_of_events; i++) {
			int fd = (int)event_list[i].ident;
			if (event_list[i].flags & EV_EOF) {
				std::cout << "Disconnect" << std::endl;
				close(fd);
			} else if (event_list[i].filter == EVFILT_READ) {
				int socket_fd = _socket.FindFd(fd);
				if (socket_fd) {
					int client_socket;
					client_socket = accept(socket_fd, NULL, NULL);
					if (client_socket == -1) throw AcceptException();
					fcntl(client_socket, F_SETFL, O_NONBLOCK);
					std::cout << "Got connection!" << std::endl;
					RegisterKevent(client_socket);
					RegisterKevent(client_socket);
					_clients[client_socket] = "";
				} else if (_clients.find(event_list[i].ident) !=
						   _clients.end()) {
					char buf[1024];
					int n = read(event_list[i].ident, buf, sizeof(buf));
					if (n <= 0) {
						if (n < 0) throw ReadException();
					} else {
						buf[n] = '\0';
						_clients[event_list[i].ident] += buf;
						std::cout << "received data from "
								  << event_list[i].ident << ": "
								  << _clients[event_list[i].ident];
					}
				}
			}
		}
	}
}
