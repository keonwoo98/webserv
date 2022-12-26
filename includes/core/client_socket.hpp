#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "kqueue_handler.hpp"
#include "request_message.hpp"
#include "response_message.hpp"
#include "server_info.hpp"
#include "server_socket.hpp"
#include "socket.hpp"
#include "udata.h"

#define BUFFER_SIZE 1024

class ClientSocket : public Socket {
   public:
	typedef ServerSocket::server_infos_type server_infos_type;

	explicit ClientSocket(const int &sock_d,
						  const server_infos_type &server_infos);
	~ClientSocket();

	bool operator<(const ClientSocket &rhs) const;

	void FindServerInfo(const RequestMessage &request);
	void PickLocationBlock(const RequestMessage &request);

	const server_infos_type &GetServerInfoVector() const;
	const ServerInfo &GetServerInfo() const;
	const int &GetLocationIndex() const;

	void FindServerInfoWithHost(const std::string &host);
	void FindLocationWithUri(const std::string &uri);

   private:
	const server_infos_type &server_infos_;
	server_infos_type::const_iterator server_info_it_;
	int location_index_;
};

#endif
