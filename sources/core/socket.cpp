#include "socket.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

Socket::Socket(int sock_d, const server_infos_type &server_infos)
: sock_d_(sock_d), server_infos_(server_infos) {}

Socket::Socket(int sock_d, const server_infos_type &server_infos,
			   const struct sockaddr_in &address)
	: sock_d_(sock_d), server_infos_(server_infos), address_(address) {}

Socket::~Socket() {
	Close();
}

const int &Socket::GetSocketDescriptor() const { return sock_d_; }

std::string Socket::HostToIpAddr(uint32_t addr) {
	std::stringstream ss;
	ss << (addr & 0xFF) << "." << ((addr >> 8) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "." << ((addr >> 24) & 0xFF);
	return ss.str();
}

std::string Socket::GetAddr() const {
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(address_.sin_addr), str, INET_ADDRSTRLEN);
	return HostToIpAddr(address_.sin_addr.s_addr);
}

std::string Socket::GetPort() const {
	std::stringstream ss;
	ss << ntohs(address_.sin_port);
	return ss.str();
}

std::string Socket::GetAddr(const int &fd) {
	struct sockaddr_in addr = {};
	socklen_t addr_len = sizeof(addr);
	getsockname(fd, (struct sockaddr *) &addr, &addr_len);
	return HostToIpAddr(addr.sin_addr.s_addr);
}

std::string Socket::GetPort(const int &fd) {
	std::stringstream ss;
	struct sockaddr_in addr = {};
	socklen_t addr_len = sizeof(addr);
	getsockname(fd, (struct sockaddr *) &addr, &addr_len);
	ss << ntohs(addr.sin_port);
	return ss.str();
}

void Socket::Close() const {
	if (sock_d_ > 0) {
		close(sock_d_);
	}
}

std::string Socket::ToString() const {
	std::stringstream ss;

	ss << "====================\n"
		<< "File Descriptor : " << sock_d_ << '\n'
		<< "Listen host : " << HostToIpAddr(address_.sin_addr.s_addr) << '\n'
		<< "Listen port : " << ntohs(address_.sin_port) << '\n'
		<< "====================\n";
	return ss.str();
}

std::ostream &operator<<(std::ostream &out, const Socket *socket) {
	out << socket->ToString();
	return out;
}
