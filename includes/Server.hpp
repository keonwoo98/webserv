#ifndef SERVER_HPP
# define SERVER_HPP

#include "socket.hpp"

#include <set>

class Server {
   public:
   static const int BACK_LOG_QUEUE_SIZE = 5;
	Server();
	~Server();
    void Listen(Socket server_socket);
	Socket AcceptClient(Socket server_socket);
	void ListenAddress(in_port_t port, in_addr_t ip_address);
	struct kevent CreateEvent(uintptr_t ident, int16_t filter, uint16_t flags,
							  uint32_t fflags, intptr_t data, void *udata);
	void run();

   private:
	std::set<Socket> listen_sockets_;
	std::set<Socket> clients_;
	int kq_fd_;	
};

#endif
