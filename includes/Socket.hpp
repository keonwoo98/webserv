#ifndef SOCKET_HPP
#define SOCKET_HPP

#ifndef DEBUG
#define DEBUG (1)
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <exception>

#include <vector>
#include <map>
#include <set>
class Socket
{
public:
	typedef std::map<int, const int> 			listening_map_t;
	typedef std::map<int, struct sockaddr_in>	client_map_t;
	typedef std::map<int, std::string>			message_map_t;

private:
	const int back_log_size_;
	const int option_on;

	listening_map_t 	listening_connections_;
	client_map_t		client_connections_;
	message_map_t		messages_for_client_;


	void setSocket(const std::vector<std::string> &ports);
	int createlisteningSocket();
	void setSocketOption(int socket_fd);
	void bindSocket(int socket_fd, int port_num);
	void setSocketNonblock(int socket_fd);

	void saveMessage(int sock_fd, const std::string & msg);
	const std::string & getMessage(int sock_fd);
	void deleteMessage(int sock_fd);
public:
	Socket();
	explicit Socket(const std::vector<std::string> &ports);
	Socket(const Socket &S);
	Socket &operator=(const Socket &S);
	~Socket();

	size_t listeningSize() const;
	size_t clientSize() const;

	void listen() const;
	int  accept(int sock_fd);
	void receive(int sock_fd);
	void send(int sock_fd);
	void close(int sock_fd);
	bool isListeningSocket(int sock_fd) const;
	bool isClientSocket(int sock_fd) const;

	const listening_map_t &	getListeningConnections() const;
	const client_map_t & 	getClientConnections() const;
	size_t	getMessageCount() const;
	void printSocketName(int sock_fd);
};

#endif
