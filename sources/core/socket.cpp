#include "socket.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

Socket::Socket(int sock_d) : sock_d_(sock_d) {}

Socket::Socket(int sock_d, const struct sockaddr_in &address)
	: sock_d_(sock_d), address_(address) {}

Socket::~Socket() { Close(); }

const int &Socket::GetSocketDescriptor() const { return sock_d_; }

std::string Socket::HostToIpAddr(uint32_t addr) {
	std::stringstream ss;
	ss << (addr & 0xFF) << "." << ((addr >> 8) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "." << ((addr >> 24) & 0xFF);
	return ss.str();
}

int Socket::GetSendBufferSize(int fd) {
	int opt_val;
	socklen_t opt_len = sizeof(opt_val);
	getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &opt_val, &opt_len);
	return opt_val;
}

std::string Socket::GetAddr() const {
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
	   << "host : " << HostToIpAddr(address_.sin_addr.s_addr) << '\n'
	   << "port : " << ntohs(address_.sin_port) << '\n'
	   << "====================\n";
	return ss.str();
}

std::ostream &operator<<(std::ostream &out, const Socket *socket) {
	out << socket->ToString();
	return out;
}
