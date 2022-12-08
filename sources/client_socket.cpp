#include "client_socket.hpp"

ClientSocket::ClientSocket(int sock_d) {
	type_ = Socket::CLIENT_TYPE;
	sock_d_ = sock_d;
}

ClientSocket::~ClientSocket() {}

void ClientSocket::PrintRequest() const {
	std::cout << parser_;
}

bool ClientSocket::RecvRequest() {
	char tmp[1024];
	int n = recv(sock_d_, tmp, sizeof(tmp), 0);
	if (n <= 0) {
		std::cerr << "read error" << std::endl;
	} else {
		tmp[n] = '\0';
		parser_.AppendMessage(tmp);
	}
	return parser_.IsDone();
}

void ClientSocket::ResetParsingState() {
	parser_.ResetState();
}

void ClientSocket::SendResponse() {
	response_.CreateMessage();
	buffer_ = response_.GetMessage();
	send(sock_d_, buffer_.c_str(), buffer_.length(), 0);
	buffer_.clear();
}
