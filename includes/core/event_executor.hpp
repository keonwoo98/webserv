#ifndef EVENT_EXECUTOR_HPP
#define EVENT_EXECUTOR_HPP

#include "client_socket.hpp"
#include "kqueue_handler.hpp"
#include "request_message.hpp"
#include "response_message.hpp"
#include "server_socket.hpp"
#include <unistd.h>

class EventExecutor {
   public:
	static const int RESPONSE_END = 0;
	static const int CLOSE = 1;
	static const int KEEP_ALIVE = 2;
	static const int ERROR = 3;
	static const int HAS_MORE = 4;

	static int AcceptClient(ServerSocket server_socket);

	static int ReceiveRequest(ClientSocket &client_socket, Udata *user_data);

	static int ReadFile(const int &fd, const int &readable_size,
						ResponseMessage &response_message);

	static int SendResponse(const ClientSocket &client_socket,
							Udata *user_data);
};

#endif
