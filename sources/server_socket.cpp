#include "server_socket.hpp"

ServerSocket::ServerSocket(int host, int port)
{
	(void)host;
	type_ = Socket::SERVER_TYPE;
	address_.sin_family = AF_INET;
	address_.sin_addr.s_addr = INADDR_ANY;
	address_.sin_port = htons(port);
}

ServerSocket::~ServerSocket() {}

void ServerSocket::ReadyToAccept() {
	CreateSocket();
	BindSocket();
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

void ServerSocket::CreateSocket() {
	int opt;

	if ((sock_d_ = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	fcntl(sock_d_, F_SETFL, O_NONBLOCK);
	opt = 1;
	if (setsockopt(sock_d_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
}

void ServerSocket::BindSocket() {
	if (bind(sock_d_, (struct sockaddr *)&address_, sizeof(address_)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
}

void ServerSocket::ListenSocket() {
	if (listen(sock_d_, 5) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
}
