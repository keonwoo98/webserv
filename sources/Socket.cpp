#include <iostream>
#include <exception>

#include <fcntl.h>

#include <netinet/in.h>

#include "../includes/Socket.hpp"


#include <arpa/inet.h> // for hton*
#include <cstdlib> // for atoi
#include <cstring> // for strcpy

void Socket::setSocket(const std::vector<std::string> &ports)
{
	size_t num_of_ports = ports.size();

	for (size_t i = 0; i < num_of_ports; i++)
	{
		int sock_fd = this->createlisteningSocket();

		this->setSocketOption(sock_fd);

		int port_num = atoi(ports[i].c_str());
		this->bindSocket(sock_fd, port_num);
		
		this->setSocketNonblock(sock_fd);

		this->listening_connections_.insert(std::make_pair(sock_fd, port_num));
	}
	if (DEBUG)
		std::cout << "listening sockets ready. : " << num_of_ports << std::endl;
}

int Socket::createlisteningSocket()
{
	int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (-1 == sock_fd)
		throw std::runtime_error("[SOCKET] socket create error");
	return (sock_fd);
}

void Socket::setSocketOption(int socket_fd)
{
	int on = 1;
	if (-1 == setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
		throw std::runtime_error("[SOCKET] socket option error");
}

void Socket::bindSocket(int socket_fd, int port_num)
{
	if (port_num < -1)
		throw std::runtime_error("[SOCKET] bad port num");
	struct sockaddr_in listen_addr = {};
	listen_addr.sin_family = PF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_addr.sin_port = htons(port_num);
	socklen_t size_of_addr = sizeof(listen_addr);
	if (-1 == bind(socket_fd, reinterpret_cast<struct sockaddr *>(&listen_addr), size_of_addr))
		throw std::runtime_error("[SOCKET] socket binding error");
	if (DEBUG) std::cout << "sockfd : " << socket_fd << " binded to " << port_num << std::endl;
}

void Socket::setSocketNonblock(int socket_fd)
{
	if (-1 == fcntl(socket_fd, F_SETFL, O_NONBLOCK))
		throw std::runtime_error("[SOCKET] socket nonblock error");
}

Socket::Socket()
	: back_log_size_(5), option_on(1) {}

/*
 * 지금은 listen할 포트로 초기화 하지만, 나중에는 다른게 올 수도 있음.
 * (conf file 같은 것)
 */
Socket::Socket(const std::vector<std::string> &ports)
	: back_log_size_(5), option_on(1)
{
	if (ports.size() < 1)
		throw std::runtime_error("[SOCKET] not enogh ports");

	this->setSocket(ports);
}

Socket::Socket(const Socket &S)
: back_log_size_(S.back_log_size_), option_on(S.option_on)
{
	listening_connections_ = S.listening_connections_;
	client_connections_ = S.client_connections_;
}

Socket & Socket::operator=(const Socket &S)
{
	if (this == &S)
		return (*this);
	listening_connections_ = S.listening_connections_;
	client_connections_ = S.client_connections_;
	return (*this);
}

Socket::~Socket()
{

}

size_t Socket::listeningSize() const
{
	return (listening_connections_.size());
}

size_t Socket::clientSize() const
{
	return (client_connections_.size());
}

void Socket::listen() const
{
	listening_map_t::const_iterator l_it = listening_connections_.begin();
	listening_map_t::const_iterator l_end = listening_connections_.end();
	for (; l_it != l_end; l_it++)
	{
		::listen(l_it->first, this->back_log_size_);
		if (DEBUG)
			std::cout << "socket #" << l_it->first << " is now listeng to : " << l_it->second << std::endl;
	}
}

int Socket::accept(int fd)
{
	struct sockaddr_in client_addr = {};
	socklen_t addr_len = sizeof(client_addr);

	int client_sock_fd = ::accept(fd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_len);
	if (-1 == client_sock_fd)
		throw std::runtime_error("[SOCKET] socket accept error");
	std::cout << "new connection occurs #" << client_sock_fd << std::endl;
	printSocketName(client_sock_fd);
	client_connections_.insert(std::make_pair(client_sock_fd, client_addr));
	/* return 후 kevent 관련 동작 필요*/
	return (client_sock_fd);
}

void Socket::receive(int sock_fd)
{
	char	buf[1024];
	std::string whole_message = "HI! this is Server!\nRECEIVED MESSAGE is :\n";
	int		recv_size;

	while (true)
	{
		recv_size = ::recv(sock_fd, buf, sizeof(buf) - 1, 0);
		if (recv_size <= 0)
			break;
		buf[recv_size] = '\0';
		whole_message.append(buf);
	}
	whole_message.append("----------------------------------------------\n");
	saveMessage(sock_fd, whole_message);
	std::cout << whole_message << std::endl;
}

void Socket::send(int sock_fd)
{
	char msg[] = "HI! this is Server!\n";
	::send(sock_fd, msg, sizeof(msg), 0);
}

void Socket::close(int sock_fd)
{
	if (isListeningSocket(sock_fd) == true)
		listening_connections_.erase(sock_fd);
	else if (isClientSocket(sock_fd) == true)
		client_connections_.erase(sock_fd);
	else
		throw std::runtime_error("[Socket] : close() bad socket descriptor");
	::close(sock_fd);
}

bool Socket::isListeningSocket(int sock_fd) const
{
	return (this->listening_connections_.find(sock_fd) != listening_connections_.end());
}

bool Socket::isClientSocket(int sock_fd) const
{
	return (this->client_connections_.find(sock_fd) != client_connections_.end());
}

const Socket::listening_map_t & Socket::getListeningConnections() const
{
	return (listening_connections_);
}
const Socket::client_map_t & Socket::getClientConnections() const
{
	return (client_connections_);
}

void Socket::saveMessage(int sock_fd, const std::string & msg)
{
	this->messages_for_client_.insert(std::make_pair(sock_fd, msg));
}

const std::string & Socket::getMessage(int sock_fd)
{
	message_map_t::iterator it = messages_for_client_.find(sock_fd);
	if (it == messages_for_client_.end())
		throw std::runtime_error("[Socket] : can't find message. no matching socket");
	return (it->second);
}

void Socket::deleteMessage(int sock_fd)
{
	messages_for_client_.erase(sock_fd);
}

size_t	Socket::getMessageCount() const
{
	return (messages_for_client_.size());
}


void Socket::printSocketName(int sock_fd)
{
	struct sockaddr_in sa = {};
	socklen_t sa_len = sizeof(sa);

	getsockname(sock_fd, reinterpret_cast<struct sockaddr *>(&sa), &sa_len);

	char ipBuffer[INET_ADDRSTRLEN] = {};
	short int portNum = 0;

	strcpy(ipBuffer, inet_ntoa(sa.sin_addr)); // inet_ntoa, not allowed
	portNum = ntohs(sa.sin_port);
	std::cout << "======sock name========" << std::endl;
	std::cout << "= IP address  : " << std::string(ipBuffer) << std::endl;
	std::cout << "= Port number : " << portNum << std::endl;
	std::cout << "=======================" << std::endl;

}
