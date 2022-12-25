#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "kqueue_handler.hpp"
#include "request_message.hpp"
#include "response_message.hpp"
#include "server_info.hpp"
#include "socket.hpp"
#include "udata.h"

#define BUFFER_SIZE 1024

class ClientSocket : public Socket {
   public:
	explicit ClientSocket(int sock_d);
	ClientSocket(int sock_d, const std::vector<ServerInfo> &server_infos);
	~ClientSocket();

	void RecvRequest();
	void SendResponse(KqueueHandler &kqueue_handler, Udata *user_data);

	bool operator<(const ClientSocket &rhs) const;

    void PickServerBlock(RequestMessage &request);
    void PickLocationBlock(RequestMessage &request);

    const ServerInfo &GetServerInfo() const;

    int GetLocationIndex() const;

    void SetServerInfo(ServerInfo &serverInfo);

    void SetLocationIndex(int locationIndex);

    void SetResolvedUri(const std::vector<std::string> &resolvedUri);

private:
	std::vector<ServerInfo> &server_infos_;
	ServerInfo &server_info_;
	int location_index_;
    std::vector<std::string> resolved_uri_;
};

bool ClientSocket::operator<(const ClientSocket &rhs) const {
	return sock_d_ < rhs.sock_d_;
}

#endif
