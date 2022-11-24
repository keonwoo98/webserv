#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

class Socket {
   public:
	Socket(int sock_fd);
	~Socket();
	static Socket CreateSocket(int domain, int type, int protocol);
	void PrintInfo();
	void SetOption(int level, int opt_name, const void *opt_val,
				   socklen_t opt_len);
	void BindAddress(sa_family_t sin_family, in_port_t port,
					 in_addr_t ip_address);
	int GetSocketFD();
	std::string readMessage();

   private:
	int sock_fd_;
};

#endif
