#include "client_socket.hpp"
#include "character_const.hpp"
#include "request_parser.hpp"

ClientSocket::ClientSocket(int sock_d, const ServerInfo &server_info)
  : Socket(server_info, Socket::CLIENT_TYPE, sock_d), 
	request_(server_info_.GetClientMaxBodySize()) {}

ClientSocket::~ClientSocket() {}

void ClientSocket::RecvRequest() {
	char tmp[BUFFER_SIZE];
	int n = recv(sock_d_, tmp, sizeof(tmp), 0);
	if (n <= 0) {
		std::cerr << "recv error" << std::endl;
		exit(1);
	} else {
		tmp[n] = '\0';
		// std::cout << C_RED << "[ClientSocket::RecvRequest] : recv line -> ";
		// std::cout << tmp << C_RESET << std::endl;
		request_parser_(request_, tmp);
	}
	if (request_.GetState() == DONE) {
		// 여기서 나머지, ServerInfo사용하는 체크 수행.
		if (RequestStartlineCheck(request_, server_info_) == true){
			RequestHeaderCheck(request_, server_info_);
		}
		std::cout << C_CYAN << "[ClientSocket::RecvRequest] : Parsing Done. result :" << C_RESET << std::endl;
		std::cout << request_ << std::endl;
		request_.Clear();
	}
}
