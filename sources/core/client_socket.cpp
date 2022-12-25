#include <unistd.h>
#include "client_socket.hpp"
#include "character_const.hpp"
#include "request_parser.hpp"


ClientSocket::ClientSocket(int sock_d, const std::vector<ServerInfo> &server_infos)
        : Socket(server_infos, Socket::CLIENT_TYPE, sock_d), request_(100000) {

}

ClientSocket::~ClientSocket() {
    if (close(sock_d_) < 0) {
        perror("close socket");
    }
}

ClientSocket::ClientSocket(int sock_d) : Socket(sock_d) {}

/*
** 구현할 예정
*/

// host 기준으로 server_block 선택하여 server_info_ 에 저장
// 예외 발생시 execption throw 해주기
void ClientSocket::PickServerBlock(RequestMessage &request) const {

}

// server_infos에서 uri를 기준으로 location의 index를 location_index_에 저장
// 예외 발생시 execption throw 해주기
void ClientSocket::PickLocationBlock(RequestMessage &request, std::string uri) {

}

const ServerInfo &ClientSocket::GetServerInfo() const {
    return server_info_;
}

int ClientSocket::GetLocationIndex() const {
    return location_index_;
}

void ClientSocket::SetServerInfo(ServerInfo &server_info) {
    server_info_ = serverInfo;
}

void ClientSocket::SetLocationIndex(int location_index) {
    location_index_ = locationIndex;
}

void ClientSocket::SetResolvedUri(const std::vector<std::string> &resolved_URI) {
    resolved_uri_ = resolvedUri;
}

