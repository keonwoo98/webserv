#include "client_socket.hpp"

#include <unistd.h>

#include "character_const.hpp"
#include "request_parser.hpp"

ClientSocket::ClientSocket(const int &sock_d,
						   const server_infos_type &server_infos,
						   const struct sockaddr_in &address)
	: Socket(sock_d, server_infos, address),
	  server_info_it_(server_infos.cbegin()),
	  location_index_(-1) {}

ClientSocket::~ClientSocket() {
	// if (close(sock_d_) < 0) {
	// 	perror("close socket");
	// }
}

bool ClientSocket::operator<(const ClientSocket &rhs) const {
	return sock_d_ < rhs.sock_d_;
}

// ClientSocket::ClientSocket(int sock_d, const Server &server_info)
//	: Socket(server_info, Socket::CLIENT_TYPE, sock_d),
//	  request_(server_info.GetClientMaxBodySize()),
//	  parser_(request_),
//	  response_(request_),
//	  prev_state_(INIT),
//	  state_(REQUEST) {

// host 기준으로 server_block 선택하여 server_info_ 에 저장
void ClientSocket::FindServerInfoWithHost(const std::string &host) { (void)host; }
// server_infos에서 uri를 기준으로 location의 index를 location_index_에 저장
// 예외 발생시 execption throw 해주기
void ClientSocket::FindLocationWithUri(const std::string &uri) { (void)uri; }

const ClientSocket::server_infos_type &ClientSocket::GetServerInfoVector() const {
	return (server_infos_);
}

const ServerInfo &ClientSocket::GetServerInfo() const {
	return *server_info_it_;
}

const int &ClientSocket::GetLocationIndex() const { return location_index_; }
