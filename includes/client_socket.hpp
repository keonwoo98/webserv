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
	enum State {
		REQUEST,
		READ_FILE,
		READ_CGI,
		RESPONSE,
		WRITE_FILE,
		WRITE_CGI,
		DONE
	};

	ClientSocket(int sock_d);
	~ClientSocket();

	const State &GetPrevState() const;
	void SetPrevState(const State &prev_state);

	const State &GetState() const;
	void SetState(const State &state);

	const int &GetFileDescriptor() const;
	void SetFileDescriptor(const int &file_d);

	void PrintRequest() const;
	void RecvRequest();
	void SendResponse();
	void ResetParsingState();

   private:
	RequestMessage request_;
	RequestParser parser_;
	ResponseMessage response_;

	State prev_state_;
	State state_;

	std::string buffer_;

	int file_d_;

	ClientSocket();
	void CreateResponse();
	void ChangeState(State state);
};

#endif
