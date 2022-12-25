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
	typedef typename ServerSocket::server_infos_type server_infos_type;

	explicit ClientSocket(int sock_d, const server_infos_type &server_infos);
	~ClientSocket();

	bool operator<(const ClientSocket &rhs) const;

	void PickServerBlock(const RequestMessage &request);
	void PickLocationBlock(const RequestMessage &request);

	const ServerInfo &GetServerInfo() const;
	const int &GetLocationIndex() const;

    void SetServerInfo(server_infos_type::const_iterator &server_info_it);
	void SetLocationIndex(const int &location_index);

   private:
	const server_infos_type &server_infos_;

	server_infos_type::const_iterator server_info_it_;
	int location_index_;
};

#endif
