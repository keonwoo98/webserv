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
	char tmp[1024];
	int n = recv(sock_d_, tmp, sizeof(tmp), 0);
	if (n <= 0) {
		if (n < 0) std::cerr << "client read error!" << std::endl;
		// disconnect_client(event_list[i].ident, clients);
	} else {
		tmp[n] = '\0';
		buffer_ += tmp;
	}
	request_.SetMessage(buffer_);
	request_.ParsingMessage();
	return n;
}

void ClientSocket::SendResponse() {
	response_.CreateMessage();
	buffer_ = response_.GetMessage();
	send(sock_d_, buffer_.c_str(), buffer_.length(), 0);
	buffer_.clear();
}
