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
	static ClientSocket *AcceptClient(KqueueHandler &kqueue_handler,
									  ServerSocket *server_socket);

	static void ReceiveRequest(KqueueHandler &kqueue_handler,
							   ClientSocket *client_socket,
							   const ServerSocket *server_socket,
							   Udata *user_data);

	static void ReadFile(KqueueHandler &kqueue_handler, const int &fd,
						 const int &readable_size, Udata *user_data);

	static void WriteReqBodyToPipe(const int &fd, Udata *user_data);

	static void ReadCgiResultFormPipe(KqueueHandler &kqueue_handler,
									  const int &fd, const int &readable_size,
									  Udata *user_data);

	static void SendResponse(KqueueHandler &kqueue_handler,
							 ClientSocket *client_socket, Udata *user_data);

   private:
	static void PrepareResponse(KqueueHandler &kqueue_handler,
								ClientSocket *client_socket, Udata *user_data);
};

#endif
