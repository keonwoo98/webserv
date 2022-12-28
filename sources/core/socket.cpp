#include "socket.hpp"

#include <arpa/inet.h>
#include <unistd.h>

Socket::Socket(int sock_d, const server_infos_type &server_infos)
: server_infos_(server_infos), sock_d_(sock_d) {}

Socket::~Socket() {
	Close();
}

const int &Socket::GetSocketDescriptor() const { return sock_d_; }

void Socket::Close() const {
	if (sock_d_ > 0) {
		close(sock_d_);
	}
}

std::ostream &operator<<(std::ostream &out, const Socket &socket) {
	int fd = socket.GetSocketDescriptor();
	struct sockaddr_in addr = {};
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
