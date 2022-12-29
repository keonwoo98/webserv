#include <sstream>

#include "webserv.hpp"
#include "event_executor.hpp"
#include "udata.hpp"
#include "core_exception.hpp"
#include "logger.hpp"

// open log files
unsigned long Webserv::access_log_fd_ = open("./logs/access.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
unsigned long Webserv::error_log_fd_ = open("./logs/error.log", O_WRONLY | O_CREAT | O_APPEND, 0644);

Webserv::Webserv(const server_configs_type &server_configs) {
	if (error_log_fd_ < 0 || access_log_fd_ < 0) {
		throw CoreException::FileOpenException();
	}

	server_configs_type::const_iterator it;
	for (it = server_configs.begin(); it != server_configs.end(); ++it) {
		ServerSocket *server = new ServerSocket(it->second);
		int sock_d = server->GetSocketDescriptor();
		servers_.insert(std::make_pair(sock_d, server)); // add listen socket to servers map

		Udata *user_data = new Udata(Udata::LISTEN, sock_d);
		kq_handler_.AddReadEvent(sock_d, user_data); // add listen socket to kqueue
	}
}

Webserv::~Webserv() {
	close(access_log_fd_);
	close(error_log_fd_);
}

/**
 * Find server socket from servers map
 * @param fd
 * @return Server Socket *
 */
ServerSocket *Webserv::FindServerSocket(int fd) {
	return servers_.find(fd)->second;
}

/**
 * Find client socket from clients map
 * @param fd
 * @return Client Socket *
 */
ClientSocket *Webserv::FindClientSocket(int fd) {
	return clients_.find(fd)->second;
}

void Webserv::RunServer() {
	while (true) {
		struct kevent event = kq_handler_.MonitorEvent(); // get event
		if (event.flags & EV_EOF) {
			delete FindClientSocket(event.ident);
			delete reinterpret_cast<Udata *>(event.udata);  // Socket is automatically removed from the kq
			clients_.erase(event.ident);
			continue;
		}
		if (event.ident == error_log_fd_ || event.ident == access_log_fd_) { // write log
			WriteLog(event);
			continue;
		}
		HandleEvent(event);
	}
}

void Webserv::WriteLog(struct kevent &event) {
	Logger *logger = reinterpret_cast<Logger *>(event.udata);
	logger->WriteLog(event);
	delete logger;
}

void Webserv::HandleEvent(struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	int event_fd = event.ident;
	int curr_state = user_data->GetState();

	switch (curr_state) {
		case Udata::LISTEN:
			HandleListenEvent(FindServerSocket(event_fd));
			return;
		case Udata::RECV_REQUEST:
			HandleReceiveRequestEvent(FindClientSocket(event_fd),
												   user_data);
			break;
		case Udata::READ_FILE:
			HandleReadFile(event_fd, event.data, user_data);
			break;	// GET
		case Udata::WRITE_TO_PIPE:
			break;	// CGI
		case Udata::READ_FROM_PIPE:
			break;	// CGI
		case Udata::SEND_RESPONSE:
			HandleSendResponseEvent(FindClientSocket(event_fd), user_data);
			break;
	}
}

void Webserv::HandleListenEvent(ServerSocket *server_socket) {
	ClientSocket *client_socket = EventExecutor::AcceptClient(kq_handler_, server_socket); // accept client
	if (client_socket == NULL) {
		return;
	}
	clients_.insert(std::make_pair(client_socket->GetSocketDescriptor(), client_socket)); // insert client to clients map
}

int Webserv::HandleReceiveRequestEvent(ClientSocket *client_socket, Udata *user_data) {
	EventExecutor::ReceiveRequest(kq_handler_, client_socket, user_data);
	return 0;
}

int Webserv::HandleReadFile(int fd, int readable_size, Udata *user_data) {
	try {
		user_data->state_ = EventExecutor::ReadFile(fd, readable_size, user_data->response_message_);
		if (user_data->state_ == Udata::SEND_RESPONSE) {
			close(fd); // delete file descriptor (remove from kqueue)
			kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
		}
	} catch (const HttpException &e) {
		kqueue_handler.AddWriteOnceEvent(error_log_fd_, new Logger(e.what())); // error_log

		ResponseMessage response_message(e.GetStatusCode(), e.GetReasonPhrase());
		user_data->response_message_ = response_message;
		user_data->state_ = Udata::SEND_RESPONSE;
		kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
	}
}

int Webserv::HandleSendResponseEvent(ClientSocket *client_socket,
									 Udata *user_data) {
	int result;
	try {
		result = EventExecutor::SendResponse(kq_handler_, client_socket, user_data);
	} catch (const std::exception &e) { // error log
		kq_handler_.AddWriteOnceEvent(error_log_fd_, new Logger(e.what()));
		result = Udata::CLOSE; // send() failed -> close
	}
	if (result == Udata::CLOSE) {
		clients_.erase(client_socket->GetSocketDescriptor()); // delete client socket from clients map
		delete client_socket; // deallocate client socket (socket closed)
	}
	return 0;
}
