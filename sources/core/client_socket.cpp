#include "client_socket.hpp"

#include <unistd.h>

ClientSocket::ClientSocket(int listen_sock_d, int sock_d, const struct sockaddr_in &address)
	: Socket(sock_d, address), listen_sock_d_(listen_sock_d) {}

ClientSocket::~ClientSocket() {
	if (close(sock_d_) < 0) {
		perror("close socket");
	}
}

bool ClientSocket::operator<(const ClientSocket &rhs) const {
	return sock_d_ < rhs.sock_d_;
}

int ClientSocket::GetListenSocketDescriptor() {
	return listen_sock_d_;
}

void ClientSocket::SetServerInfo(ServerInfo server_info) {
	server_info_ = server_info;
}
