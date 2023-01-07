#ifndef EVENT_EXECUTOR_HPP
#define EVENT_EXECUTOR_HPP

#include "client_socket.hpp"
#include "kqueue_handler.hpp"
#include "request_message.hpp"
#include "response_message.hpp"
#include "resolve_uri.hpp"
#include "server_socket.hpp"
#include <unistd.h>

class EventExecutor {
   public:
	static void AcceptClient(KqueueHandler &kqueue_handler, struct kevent &event);
	static void ReceiveRequest(KqueueHandler &kqueue_handler, const struct kevent &event);
	static void ReadFile(KqueueHandler &kqueue_handler, struct kevent &event);
	static void WriteFile(KqueueHandler &kqueue_handler, struct kevent &event);
	static void WriteReqBodyToPipe(KqueueHandler &kqueue_handler, struct kevent &event);
	static void ReadCgiResultFromPipe(KqueueHandler &kqueue_handler, struct kevent &event);
	static void SendResponse(KqueueHandler &kqueue_handler, struct kevent &event);
   private:
	static int CheckErrorPages(ClientSocket *client_socket, Udata *user_data);
	static void HandleAutoIndex(KqueueHandler &kqueue_handler, Udata *user_data, const std::string resolved_uri);
	static void HandleStaticFile(KqueueHandler &kqueue_handler, Udata *user_data);
	static void HandleRequestResult(ClientSocket *client_socket, Udata *user_data, KqueueHandler &kqueue_handler);
};

#endif
