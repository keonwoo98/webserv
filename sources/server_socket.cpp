#include "server_socket.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

const int ServerSocket::BACK_LOG_QUEUE = 5;

ServerSocket::ServerSocket(const std::string &host, const std::string &port) {
	type_ = Socket::SERVER_TYPE;
	CteateSocket(host, port);
}

ServerSocket::~ServerSocket() {}

void ServerSocket::ReadyToAccept() {
	ListenSocket();
}

int ServerSocket::AcceptClient() {
	int accept_d;
	if ((accept_d = accept(sock_d_, NULL, NULL)) < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}
	fcntl(accept_d, F_SETFL, O_NONBLOCK);
	return accept_d;
}

void ServerSocket::CteateSocket(const std::string &host, const std::string &port) {
	int status;
	struct addrinfo hints;
	struct addrinfo *addr_list;	 // 결과를 저장할 변수

	memset(&hints, 0, sizeof(hints));  // hints 구조체의 모든 값을 0으로 초기화
	hints.ai_family = PF_INET;		  // IPv4
	hints.ai_socktype = SOCK_STREAM;  // TCP stream socket

	status = getaddrinfo(host.c_str(), port.c_str(), &hints, &addr_list);
	if (status != 0) {
		std::cout << gai_strerror(status) << std::endl;
	}

	sock_d_ = BindSocket(addr_list);
	freeaddrinfo(addr_list);

	if (sock_d_ < -1) {
		perror("bind failed");
	}
}

int ServerSocket::BindSocket(struct addrinfo *result) {
	int sfd = -1;
	int opt = 1;
	struct addrinfo *rp;

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
		if (sfd == -1) {
			continue;
		}
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
			break; /* Success */
		}
		if (close(sfd) < 0) {
			perror("close failed");
			exit(EXIT_FAILURE);
		}
	}
	return sfd;
}

void ServerSocket::ListenSocket() {
	if (listen(sock_d_, ServerSocket::BACK_LOG_QUEUE) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
}
