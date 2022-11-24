#include "server_socket.hpp"

ServerSocket::ServerSocket() {}

int ServerSocket::CreateSocket(int port) {
	int socket_fd;
	struct sockaddr_in address;
	int opt = 1;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		throw SocketException();
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
		throw SetsockoptException();
	if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
		throw BindException();
	if (listen(socket_fd, 3) == -1) throw ListenException();
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1) throw FcntlException();
	_listening_sockets.insert(socket_fd);
	return socket_fd;
}

int ServerSocket::FindFd(int fd) {
	std::set<int>::iterator it;

	it = _listening_sockets.find(fd);
	if (it != _listening_sockets.end())
		return *it;
	else
		return 0;
}
