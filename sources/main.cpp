#include "socket.hpp"
#include "server.hpp"
#include <sys/socket.h>

int main(void) {
	Socket socket = Socket::CreateSocket(PF_INET, SOCK_STREAM, 0);
	Server server = Server();
	server.run();
}
