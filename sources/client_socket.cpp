#include "client_socket.hpp"

ClientSocket::ClientSocket(int sock_d) {
	type_ = Socket::CLIENT_TYPE;
	sock_d_ = sock_d;
}

ClientSocket::~ClientSocket() {}

const std::string &ClientSocket::GetRequest() const {
	return request_;
}

int ClientSocket::RecvRequest() {
	char buf[1024];
	int n = recv(sock_d_, buf, sizeof(buf), 0);
	if (n <= 0) {
		if (n < 0) std::cerr << "client read error!" << std::endl;
		// disconnect_client(event_list[i].ident, clients);
	} else {
		buf[n] = '\0';
		request_ += buf;
	}
	return n;
}

void ClientSocket::SendResponse() {
	response_ =
		"HTTP/1.1 200 OK\r\n"
		"Server: webserv\r\n"
		"Date: Thu, 24 Nov 2022 09:03:29 GMT\r\n"
		"Last-Modified: Tue, 19 Jul 2022 14:05:34 GMT\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 16\r\n"
		"ETag: \"62d6ba2e-267\"\r\n"
		"Accept-Ranges: bytes\r\n"
		"\r\n"
		"<html>hi</html>\n";
	send(sock_d_, response_.c_str(), response_.length(), 0);
	request_.clear();
}