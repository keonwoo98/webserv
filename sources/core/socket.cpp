#include "socket.hpp"

#include <arpa/inet.h>
#include <unistd.h>

Socket::Socket(const std::vector<ServerInfo> &server_infos, int type) : server_infos_(server_infos), type_(type), sock_d_(), address_() {
}

Socket::Socket(const std::vector<ServerInfo> &server_infos, int type, int sock_d)
	: server_infos_(server_infos), type_(type), sock_d_(sock_d), address_() {}

Socket::~Socket() {}

const int &Socket::GetType() const { return type_; }

const int &Socket::GetSocketDescriptor() const { return sock_d_; }

void Socket::Close() const {
	if (close(sock_d_) < 0) {
		perror("close: ");
	}
}

const std::vector<ServerInfo> &Socket::GetServerInfos() const {
	return server_infos_;
}

std::ostream &operator<<(std::ostream &out, const Socket *socket) {
	int fd = socket->GetSocketDescriptor();
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	int state = getsockname(fd, (struct sockaddr *) &addr, &addr_len);
	if (state < 0) {
		// Error
		perror("getsockname : ");
		return out;
	}
	out << "====================\n"
		<< "File Descriptor : " << fd << '\n'
		<< "host : " << inet_ntoa(addr.sin_addr) << '\n'  // 허용 함수 아님
		<< "port : " << ntohs(addr.sin_port) << '\n'
		<< "====================\n";
	return out;
}
