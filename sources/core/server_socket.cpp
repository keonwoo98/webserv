#include "server_socket.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "core_exception.hpp"

const int ServerSocket::BACK_LOG_QUEUE = 5;

ServerSocket::ServerSocket(const server_infos_type &server_infos)
	: Socket(-1, server_infos) {
	std::string host = server_infos.begin()->GetHost();
	std::string port = server_infos.begin()->GetPort();
	CreateSocket(host, port);
}

ServerSocket::~ServerSocket() {}

const std::vector<ServerInfo> &ServerSocket::GetServerInfos() const {
	return server_infos_;
}

bool ServerSocket::operator<(const ServerSocket &rhs) const {
	return sock_d_ < rhs.sock_d_;
}

ClientSocket *ServerSocket::AcceptClient() {
	struct sockaddr_in clientaddr;
	socklen_t clientaddr_len = sizeof(clientaddr);
	int fd = accept(sock_d_, (struct sockaddr *)&clientaddr, &clientaddr_len);
	if (fd < 0) {
		throw std::exception(); // System error exception 필요
	}
	fcntl(fd, F_SETFL, O_NONBLOCK);
	// char str[INET_ADDRSTRLEN];
	// inet_ntop(AF_INET, &(clientaddr.sin_addr), str, INET_ADDRSTRLEN);
	// std::cout << "ADDR: " << str << std::endl;
	return new ClientSocket(fd, server_infos_, clientaddr);
}

void ServerSocket::CreateSocket(const std::string &host, const std::string &port) {
	struct addrinfo *addr_list = GetAddrInfos(host, port);
	Bind(addr_list);
	freeaddrinfo(addr_list);
	if (sock_d_ < 0) {
		perror("bind");
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
