#include "client_socket.hpp"

#include <unistd.h>

#include "character_const.hpp"
#include "request_parser.hpp"

ClientSocket::ClientSocket(int sock_d, const server_infos_type &server_infos)
	: Socket(sock_d),
	  server_infos_(server_infos) {}	// server_info_[0] is for initialize

ClientSocket::~ClientSocket() {
    if (close(sock_d_) < 0) {
        perror("close socket");
    }
}

bool ClientSocket::operator<(const ClientSocket &rhs) const {
	return sock_d_ < rhs.sock_d_;
}

//ClientSocket::ClientSocket(int sock_d, const Server &server_info)
//	: Socket(server_info, Socket::CLIENT_TYPE, sock_d),
//	  request_(server_info.GetClientMaxBodySize()),
//	  parser_(request_),
//	  response_(request_),
//	  prev_state_(INIT),
//	  state_(REQUEST) {

// host 기준으로 server_block 선택하여 server_info_ 에 저장
void ClientSocket::PickServerBlock(const RequestMessage &request) const {}

void ClientSocket::PickLocationBlock(const RequestMessage &request) const {}

// server_infos에서 uri를 기준으로 location의 index를 location_index_에 저장
// 예외 발생시 execption throw 해주기
void ClientSocket::PickLocationBlock(const RequestMessage &request) {

}

const ServerInfo &ClientSocket::GetServerInfo() const {
    return *server_info_it_;
}

const int &ClientSocket::GetLocationIndex() const {
    return location_index_;
}

void ClientSocket::SetServerInfo(server_infos_type::const_iterator &server_info_it) {
    server_info_it_ = server_info_it;
}

void ClientSocket::SetLocationIndex(const int &location_index) {
    location_index_ = location_index;
}

void ClientSocket::SetResolvedUri(const std::vector<std::string> &resolved_uri) {
    resolved_uri_ = resolved_uri;
}

