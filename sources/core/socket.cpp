#include "socket.hpp"

#include <arpa/inet.h>
#include <unistd.h>

Socket::Socket(int sock_d, const server_infos_type &server_infos)
: sock_d_(sock_d), server_infos_(server_infos) {}

Socket::Socket(int sock_d, const server_infos_type &server_infos,
			   const struct sockaddr_in &address)
	: sock_d_(sock_d), server_infos_(server_infos), address_(address) {}

Socket::~Socket() {
	Close();
}

int Socket::GetSocketDescriptor() const { return sock_d_; }

std::string Socket::GetHost() const {
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(address_.sin_addr), str, INET_ADDRSTRLEN);
	return str;
}

std::string Socket::GetPort() const {
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(address_.sin_port), str, INET_ADDRSTRLEN);
	return str;
}

void Socket::Close() const {
	if (sock_d_ > 0) {
		close(sock_d_);
	}
}

std::ostream &operator<<(std::ostream &out, const Socket *socket) {
	int fd = socket->GetSocketDescriptor();
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
		<< "Listen host : " << inet_ntoa(addr.sin_addr) << '\n'  // 허용 함수 아님
		<< "Listen port : " << ntohs(addr.sin_port) << '\n'
		<< "====================\n";
	return out;
}
