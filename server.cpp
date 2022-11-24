#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

int CreateSocket(int domain, int type, int protocol) {
	int sock_fd = socket(domain, type, protocol);
	if (sock_fd < 0) {
		perror("socket error: ");
		// 예외처리
	}
	return sock_fd;
}

void SetSocketOption(int sd, int level, int opt_name, const void *opt_val,
					 socklen_t opt_len) {
	int state = setsockopt(sd, level, opt_name, opt_val, sizeof(opt_val));
	if (state < 0) {
		perror("setsocketopt error: ");
		// 예외처리
	}
}

void FillSocket(sockaddr_in &sock_addr, sa_family_t sin_family, in_port_t port,
				in_addr_t ip_address) {
	bzero(&sock_addr, sizeof(sock_addr));			// 0으로 초기화
	sock_addr.sin_family = sin_family;				// IPv4
	sock_addr.sin_port = htons(port);				// 포트 번호
	sock_addr.sin_addr.s_addr = htonl(ip_address);	// IP 주소
}

void BindSocket(int server_sock_fd, const sockaddr *addr, socklen_t addrlen) {
	int state = bind(server_sock_fd, addr, addrlen);
	if (state < 0) {
		perror("bind error : ");
		// 예외처리
	}
}

void SetListenSocket(int sock_fd) {
	const int BACK_LOG_QUEUE = 5;  // 상수
	int state = listen(sock_fd, BACK_LOG_QUEUE);
	if (state < 0) {
		perror("listen error: ");
		// 예외처리
	}
}

int AcceptClient(int server_sock_fd, sockaddr_in &client_addr) {
	int client_sock_fd;
	socklen_t client_sock_len = sizeof(client_addr);

	client_sock_fd = accept(server_sock_fd, (struct sockaddr *)&client_addr,
							&client_sock_len);
	if (client_sock_fd < 0) {
		perror("accept error: ");
		return (EXIT_FAILURE);
	}
	return client_sock_fd;
}

void PrintSocketInfo(int sock_fd) {
	struct sockaddr_in sock_addr;
	socklen_t sock_addr_len = sizeof(sock_addr);

	int state =
		getsockname(sock_fd, (struct sockaddr *)&sock_addr, &sock_addr_len);
	if (state < 0) {
		// 예외처리
	}

	std::cout << "IP Address : " << inet_ntoa(sock_addr.sin_addr) << std::endl;
	std::cout << "port number : " << ntohs(sock_addr.sin_port) << std::endl;
}

int CreateKqueue() {
	int kq_fd = kqueue();
	if (kq_fd < 0) {
		perror("kqueue error: ");
		// 예외처리
	}
	return kq_fd;
}

struct kevent CreateEvent(uintptr_t ident, int16_t filter, uint16_t flags,
						  uint32_t fflags, intptr_t data, void *udata) {
	struct kevent event;

	EV_SET(&event, ident, filter, flags, fflags, data, udata);
	return event;
}

int CreateServer(in_port_t port, in_addr_t ip_address) {
	int server_sock_fd = CreateSocket(PF_INET, SOCK_STREAM, 0);

	int opt = 1;  // !!
	SetSocketOption(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt,
					sizeof(opt));

	// 주소, port 번호 채워넣기
	struct sockaddr_in server_addr;
	FillSocket(server_addr, AF_INET, port, ip_address);	 // 8080 IN_ADDR_ANY

	// 클라언트가 서버의 주소를 알 수 있도록 해준다.
	BindSocket(server_sock_fd, (struct sockaddr *)&server_addr,
			   sizeof(server_addr));

	// NON_BLOCK
	fcntl(server_sock_fd, F_SETFL, O_NONBLOCK);
	
	// 연결 요청 대기상태로 만든다.
	SetListenSocket(server_sock_fd);

	return server_sock_fd;
}

void AddServersEventToChangeList(std::vector<struct kevent> &change_list,
								 std::set<int> &servers) {
	std::set<int>::iterator it = servers.begin();
	while (it != servers.end()) {
		struct kevent event = CreateEvent(*it, EVFILT_READ, EV_ADD, 0, 0, NULL);
		change_list.push_back(event);
		it++;
	}
}

int AccepClient(int server_sock_fd) {
	int client_sock_fd = accept(server_sock_fd, NULL, NULL);
	if (client_sock_fd < 0) {
		// error
	}
	fcntl(client_sock_fd, F_SETFL, O_NONBLOCK);
	std::cout << "client connected!" << std::endl;
	PrintSocketInfo(client_sock_fd);
	return client_sock_fd;
}

std::string ReceiveMessageFromSocket(int sock_fd) {
	const int BUFFER_SIZE = 1024;
	std::string message;

	char buff[BUFFER_SIZE];
	int count = recv(sock_fd, buff, BUFFER_SIZE, 0);
	if (count < 0) {
		perror("recv error: ");
	}
	message.append(buff);
	return message;
}

const int EVENT_LIST_SIZE = 8;

int main(void) {
	std::vector<struct kevent> change_list;
	std::set<int> servers;
	std::set<int> clients;
	struct kevent event_list[EVENT_LIST_SIZE];

	servers.insert(CreateServer(8080, INADDR_ANY));
	servers.insert(CreateServer(8081, INADDR_ANY));
	servers.insert(CreateServer(8082, INADDR_ANY));
	AddServersEventToChangeList(change_list, servers);

	int kq = CreateKqueue();

	while (true) {
		int number_of_events =
			kevent(kq, (struct kevent *)change_list.data(), change_list.size(),
				   event_list, EVENT_LIST_SIZE, NULL);
		// exception
		change_list.clear();
		for (int i = 0; i < number_of_events; i++) {
			int fd = event_list[i].ident;

			if (event_list[i].flags & EV_EOF) {
				std::cout << "Disconnect" << std::endl;
				close(fd);
			} else if (event_list[i].filter == EVFILT_READ) {
				// READ_EVENT
				std::set<int>::iterator it;
				if ((it = servers.find(fd)) != servers.end()) {
					int client_sock_fd = AccepClient(*it);
					change_list.push_back(CreateEvent(
						client_sock_fd, EVFILT_READ, EV_ADD, 0, 0, NULL));
					clients.insert(client_sock_fd);
				} else if (clients.find(fd) != clients.end()) {
					std::string message = ReceiveMessageFromSocket(fd);
					std::cout << "message : " << message << std::endl;
					change_list.push_back(CreateEvent(
						fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL));
				}
			} else if (event_list[i].filter == EVFILT_WRITE) {
				// WRITE EVENT
				std::string response =
					"HTTP/1.1 404 Not Found\n"
					"Server: nginx/0.8.54\n"
					"Date: Mon, 02 Jan 2012 02:33:17 GMT\n"
					"Content-Type: text/html\n"
					"Content-Length: 147\n"
					"Connection: close";

				int count = send(fd, response.c_str(), response.length(), 0);
				// TODO
			}
		}
	}
	return (EXIT_SUCCESS);
}
