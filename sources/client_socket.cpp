#include "client_socket.hpp"
const int ClientSocket::BUFFER_SIZE = 1024;

ClientSocket::ClientSocket(int sock_d) {
	type_ = Socket::CLIENT_TYPE;
	sock_d_ = sock_d;
}

ClientSocket::~ClientSocket() {}

const std::string &ClientSocket::GetMessage() const {
	return message_;
}

int ClientSocket::ReadMessage() {
	char buf[BUFFER_SIZE];
	int n = recv(sock_d_, buf, BUFFER_SIZE, 0);

	if (n <= 0) {
		if (n < 0) std::cerr << "client read error!" << std::endl;
		// disconnect_client(event_list[i].ident, clients);
	} else {
		buf[n] = '\0';
		message_ += buf;
	}
	return n;
}
