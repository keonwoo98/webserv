#include "client_socket.hpp"

ClientSocket::ClientSocket(int sock_d) {
	type_ = Socket::CLIENT_TYPE;
	sock_d_ = sock_d;
}

ClientSocket::~ClientSocket() {}

const std::string ClientSocket::GetRequest() {
	return request_.GetMessage();
}

int ClientSocket::RecvRequest() {
	int n = request_.RecvMessage(sock_d_);
	request_.ParsingMessage();
	return n;
}

void ClientSocket::SendResponse() {
	response_.CreateMessage();
	std::string message = response_.GetMessage();
	send(sock_d_, message.c_str(), message.length(), 0);
}
