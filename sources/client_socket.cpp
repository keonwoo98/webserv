#include "client_socket.hpp"
#include <arpa/inet.h>

const int ClientSocket::BUFFER_SIZE = 1024;

ClientSocket::ClientSocket(int sock_d) {
	type_ = Socket::CLIENT_TYPE;
	sock_d_ = sock_d;
}

ClientSocket::~ClientSocket() {}


const std::string &ClientSocket::GetMessage() const { return message_; }

void ClientSocket::clear() {
	message_.clear();
}

int ClientSocket::ReadMessage() {
	char buf[BUFFER_SIZE];
	int n = recv(sock_d_, buf, BUFFER_SIZE, 0);
	if (n <= 0) {
		if (n < 0) {
			std::cerr << "client read error!" << std::endl;
			return n;
		}
		// disconnect_client(event_list[i].ident, clients);
	}
	message_.append(buf, n);
	return n;
}