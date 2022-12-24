#include "server_socket.hpp"
#include "core_exception.h"

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>

const int ServerSocket::BACK_LOG_QUEUE = 5;

ServerSocket::ServerSocket() : Socket(Socket::SERVER_TYPE) {
}

ServerSocket::ServerSocket(const std::string &host, const std::string &port) : Socket(Socket::SERVER_TYPE) {
	CreateSocket(host, port);
}

ServerSocket::~ServerSocket() {
}

int ServerSocket::AcceptClient() {
	int fd;
	if ((fd = accept(sock_d_, NULL, NULL)) < 0) {
		perror("accept");
		return (EXIT_FAILURE);
	}
	fcntl(fd, F_SETFL, O_NONBLOCK);
	return fd;
}

void ServerSocket::CreateSocket(const std::string &host, const std::string &port) {
	struct addrinfo *addr_list = GetAddrInfo(host, port);
	Bind(addr_list);
	freeaddrinfo(addr_list);
	if (sock_d_ < 0) {
		perror("bind");
		throw CoreException::BindException();
	}
	Listen();
}

struct addrinfo *ServerSocket::GetAddrInfo(const std::string &host, const std::string &port) {
	struct addrinfo hints = {};
	hints.ai_family = PF_INET;            // IPv4
	hints.ai_socktype = SOCK_STREAM;    // TCP stream socket
	hints.ai_flags = AI_PASSIVE;        // for server bind
	struct addrinfo *addr_list;
	int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &addr_list);
	if (status != 0) {
		std::cerr << gai_strerror(status) << std::endl;
		throw CoreException::GetAddrInfoException();
	}
	return addr_list;
}

void ServerSocket::Bind(struct addrinfo *result) {
	struct addrinfo *curr;
	int opt = 1;
	sock_d_ = -1;

	for (curr = result; curr != NULL; curr = curr->ai_next) {
		sock_d_ = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
		if (sock_d_ < 0) {
			continue;
		}
		if (setsockopt(sock_d_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
			perror("setsockopt");
			Close();
			continue;
		}
		if (bind(sock_d_, curr->ai_addr, curr->ai_addrlen) == 0) {
			break; /* Success */
		}
		Close();
		sock_d_ = -1;
	}
}

void ServerSocket::Listen() {
	if (listen(sock_d_, ServerSocket::BACK_LOG_QUEUE) < 0) {
		perror("listen");
		throw CoreException::ListenException();
	}
}
