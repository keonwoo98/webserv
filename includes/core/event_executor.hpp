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
	static ClientSocket *AcceptClient(KqueueHandler &kqueue_handler, ServerSocket *server_socket);

	static int ReceiveRequest(KqueueHandler &kqueue_handler, ClientSocket *client_socket, Udata *user_data);

	static int ReadFile(int fd, int readable_size,
						ResponseMessage &response_message);

	static int SendResponse(KqueueHandler &kqueue_handler, ClientSocket *client_socket, Udata *user_data);
};

#endif
