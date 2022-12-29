#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "kqueue_handler.hpp"
#include "request_message.hpp"
#include "response_message.hpp"
#include "server_info.hpp"
#include "server_socket.hpp"
#include "socket.hpp"
#include "udata.hpp"

class ClientSocket : public Socket {
   public:
	ClientSocket(int listen_sock_d, int sock_d, const struct sockaddr_in &address);
	~ClientSocket();

	bool operator<(const ClientSocket &rhs) const;

	const server_infos_type &GetServerInfoVector() const;
	const ServerInfo &GetServerInfo() const;
	int GetLocationIndex() const;
	int GetListenSocketDescriptor();
	void SetServerInfo(ServerInfo server_info);

	void FindServerInfoWithHost(const std::string &host);
	void FindLocationWithUri(const std::string &uri);

   private:
	ServerInfo server_info_;
	int listen_sock_d_;
};

#endif
