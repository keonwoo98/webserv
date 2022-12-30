#include "client_socket.hpp"

#include <unistd.h>

#include "character_const.hpp"
#include "request_parser.hpp"

ClientSocket::ClientSocket(int sock_d, int server_d, const struct sockaddr_in &address)
	: Socket(sock_d, address), server_d_(server_d) {}

ClientSocket::~ClientSocket() {
	if (close(sock_d_) < 0) {
		perror("close socket");
	}
}

bool ClientSocket::operator<(const ClientSocket &rhs) const {
	return sock_d_ < rhs.sock_d_;
}

const ServerInfo &ClientSocket::GetServerInfo() const {
	return server_info_;
}

int ClientSocket::GetServerFd() const {
	return server_d_;
}

void ClientSocket::SetServerInfo(const ServerInfo &single_server_info) {
	server_info_ = single_server_info;
}

void ClientSocket::SetLocationIndex(int index) {
	server_info_.SetLocationIndex(index);
}

