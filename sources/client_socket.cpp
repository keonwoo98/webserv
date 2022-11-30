#include "client_socket.hpp"

ClientSocket::ClientSocket(int sock_d) {
	type_ = Socket::CLIENT_TYPE;
	sock_d_ = sock_d;
}

ClientSocket::~ClientSocket() {}

const std::string ClientSocket::GetRequest() {
	return request_.GetHeader() + request_.GetBody();
}

int ClientSocket::RecvRequest() {
	std::string message;
	char buf[1024];
	int n = recv(sock_d_, buf, sizeof(buf), 0);
	if (n <= 0) {
		if (n < 0) std::cerr << "client read error!" << std::endl;
		// disconnect_client(event_list[i].ident, clients);
	} else {
		buf[n] = '\0';
		message += buf;
	}
	request_.ParsingMessage(message);
	return n;
}

void ClientSocket::SendResponse() {
	response_.CreateMessage();
	std::string message = response_.GetHeader() + response_.GetBody();
	send(sock_d_, message.c_str(), message.length(), 0);
}
