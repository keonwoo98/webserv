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

	explicit ClientSocket(const int &sock_d,
						   const server_infos_type &server_infos,
						   const struct sockaddr_in &address);
	~ClientSocket();

	bool operator<(const ClientSocket &rhs) const;

	const server_infos_type &GetServerInfoVector() const;
	const ServerInfo &GetServerInfo() const;
	int GetLocationIndex() const;

	void FindServerInfoWithHost(const std::string &host);
	void FindLocationWithUri(const std::string &uri);

   private:
	server_infos_type::const_iterator server_info_it_;
	int location_index_;
};

#endif
