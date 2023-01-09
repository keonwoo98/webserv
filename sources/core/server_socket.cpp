#include "server_socket.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include "core_exception.hpp"

const int ServerSocket::BACK_LOG_QUEUE = 128;

ServerSocket::ServerSocket(const server_infos_type &server_infos)
	: Socket(-1), server_infos_(server_infos) {
	std::string host = server_infos.begin()->GetHost();
	std::string port = server_infos.begin()->GetPort();
	CreateSocket(host, port);
}

ServerSocket::~ServerSocket() {}

bool ServerSocket::operator<(const ServerSocket &rhs) const {
	return sock_d_ < rhs.sock_d_;
}

int SetSocketOpt(int fd) {
	int result = 0;
	int opt_val = 1;
	int opt_len = sizeof(opt_val);

	opt_val = 1;
	result = setsockopt(fd, SOL_SOCKET, SO_SNDLOWAT, &opt_val, opt_len);
	if (result <= 0) {
		return result;
	}
	opt_val = 1;
	result = setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &opt_val, opt_len);
	return result;
}

ClientSocket *ServerSocket::AcceptClient() {
	struct sockaddr_in addr = {};
	socklen_t addr_len = sizeof(addr);
	int fd = accept(sock_d_, (struct sockaddr *) &addr, &addr_len);
	if (fd < 0) {
		return NULL;
	}
	fcntl(fd, F_SETFL, O_NONBLOCK);
	if (SetSocketOpt(fd) < 0) {
		close(fd);
		return NULL;
	}
	return new ClientSocket(fd, sock_d_, addr);
}

void ServerSocket::CreateSocket(const std::string &host, const std::string &port) {
	struct addrinfo *addr_list = GetAddrInfos(host, port);
	Bind(addr_list);
	freeaddrinfo(addr_list);
	if (sock_d_ < 0) {
		throw CoreException::BindException();
	}
	Listen();
}

struct addrinfo *ServerSocket::GetAddrInfos(const std::string &host,
											const std::string &port) {
	struct addrinfo hints = {};
	hints.ai_family = PF_INET;			// IPv4
	hints.ai_socktype = SOCK_STREAM;	// TCP stream socket
	hints.ai_flags = AI_PASSIVE;		// for server bind
	struct addrinfo *addr_list;
	int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &addr_list);
	if (status != 0) {
		std::cerr << gai_strerror(status) << std::endl;
		throw CoreException::GetAddrInfoException();
	}
	return addr_list;
}

void ServerSocket::Bind(struct addrinfo *result) {
	struct addrinfo *curr;
	int opt = 1;
	// sock_d_ = -1;

	for (curr = result; curr != NULL; curr = curr->ai_next) {
		sock_d_ = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
		if (sock_d_ < 0) {
			continue;
		}
		if (setsockopt(sock_d_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
			perror("setsockopt");
			Close();
			continue;
		}
		if (bind(sock_d_, curr->ai_addr, curr->ai_addrlen) == 0) {
			address_ = *(struct sockaddr_in *) (curr->ai_addr);
			break; /* Success */
		}
		Close();
		sock_d_ = -1;
	}
}

void ServerSocket::Listen() {
	if (listen(sock_d_, ServerSocket::BACK_LOG_QUEUE) < 0) {
		perror("listen");
		throw CoreException::ListenException();
	}
}

const ServerSocket::server_infos_type &ServerSocket::GetServerInfos() const {
	return server_infos_;
}
