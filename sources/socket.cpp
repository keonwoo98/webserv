#include "socket.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

Socket::Socket(int sock_fd) : sock_fd_(sock_fd) {}

Socket Socket::CreateSocket(int domain, int type, int protocol) {
	return Socket(socket(domain, type, protocol));
}

int Socket::GetSocketFD() {
	return sock_fd_;
}

void Socket::PrintInfo() {
	struct sockaddr_in addr;
	socklen_t sock_addr_len = sizeof(addr);

	int state = getsockname(sock_fd_, (struct sockaddr *)&addr, &sock_addr_len);
	if (state < 0) {
		// 예외처리
	}

	std::cout << "IP Address : " << inet_ntoa(addr.sin_addr) << std::endl;
	std::cout << "port number : " << ntohs(addr.sin_port) << std::endl;
}

void Socket::SetOption(int level, int opt_name, const void *opt_val,
					   socklen_t opt_len) {
	int result = setsockopt(sock_fd_, level, opt_name, opt_val, sizeof(opt_val));
	if (result < 0) {
		perror("setsocketopt error: ");
		// 예외처리
	}
}

void Socket::BindAddress(sa_family_t sin_family, in_port_t port,
						 in_addr_t ip_address) {
	struct sockaddr_in addr;

	bzero(&addr, sizeof(addr));
	addr.sin_family = sin_family;			   // IPv4
	addr.sin_port = htons(port);			   // 포트 번호
	addr.sin_addr.s_addr = htonl(ip_address);  // IP 주소

	int result = bind(sock_fd_, (struct sockaddr *)&addr, sizeof(addr));
	if (result < 0) {
		perror("bind error : ");
		// 예외처리
	}
}

std::string Socket::readMessage() {
	const int BUFFER_SIZE = 1024;
	std::string message;

	char buff[BUFFER_SIZE];
	int count = recv(sock_fd_, buff, BUFFER_SIZE, 0);
	if (count < 0) {
		perror("recv error: ");
		// 예외처리
	}
	message.append(buff);
	return message;
}

Socket::~Socket() {
	close(sock_fd_);
}
