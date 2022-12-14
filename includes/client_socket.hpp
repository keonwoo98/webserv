#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <vector>
#include <ctime>

#include "socket.hpp"
#include "request_parser.hpp"
#include "response_message.hpp"

class ClientSocket : public Socket {
   public:
	static const int BUFFER_SIZE;
	ClientSocket(int sock_d);
	~ClientSocket();

	void PrintRequest() const;
	bool RecvRequest();
	void SendResponse();
	void ResetParsingState();

   private:
	RequestParser parser_;
	RequestMessage request_;
	ResponseMessage response_;

	std::string buffer_;

	ClientSocket();
	void CreateResponse();
};

#endif
