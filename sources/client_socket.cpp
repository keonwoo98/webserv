#include "client_socket.hpp"

ClientSocket::ClientSocket(int sock_d) {
	type_ = Socket::CLIENT_TYPE;
	sock_d_ = sock_d;
}

ClientSocket::~ClientSocket() {}

const std::string &ClientSocket::GetMessage() const {
	return message_;
}

void ClientSocket::ReadMessage() {
	char buf[1024];
	int n = read(sock_d_, buf, sizeof(buf));
	if (n <= 0) {
		if (n < 0) std::cerr << "client read error!" << std::endl;
		// disconnect_client(event_list[i].ident, clients);
	} else {
		buf[n] = '\0';
		message_ += buf;
	}
}
