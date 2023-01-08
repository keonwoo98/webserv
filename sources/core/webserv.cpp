#include "webserv.hpp"
#include "event_executor.hpp"
#include "udata.hpp"
#include "core_exception.hpp"
#include "logger.hpp"
#include "kevent_util.hpp"

// open logs files
int Webserv::access_log_fd_ = open("./logs/access.logs", O_WRONLY | O_CREAT | O_APPEND, 0644);
int Webserv::error_log_fd_ = open("./logs/error.logs", O_WRONLY | O_CREAT | O_APPEND, 0644);
// Sockets
Webserv::servers_type Webserv::servers_;
Webserv::clients_type Webserv::clients_;

Webserv::Webserv(const server_configs_type &server_configs) {
	if (error_log_fd_ < 0 || access_log_fd_ < 0) {
		throw CoreException::FileOpenException();
	}
	CreateListenSockets(server_configs);
}

void Webserv::CreateListenSockets(const server_configs_type &server_configs) {
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
	Webserv::servers_type::const_iterator it = servers_.find(fd);
	if (it == servers_.end()) {
		return NULL;
	}
	return servers_.find(fd)->second;
}

/**
 * Find client socket from clients map
 * @param fd
 * @return Client Socket *
 */
ClientSocket *Webserv::FindClientSocket(int fd) {
	Webserv::clients_type::const_iterator it = clients_.find(fd);
	if (it == clients_.end()) {
		return NULL;
	}
	return clients_.find(fd)->second;
}

void Webserv::DeleteProcEvent(const struct kevent &event) {
	waitpid((pid_t) event.ident, 0, WNOHANG);
	kq_handler_.DeleteEvent(event);
}

void Webserv::DeleteClient(const struct kevent &event) {
	ClientSocket *client_socket = FindClientSocket((int) event.ident);

	clients_.erase(client_socket->GetSocketDescriptor());
	delete client_socket;
	delete reinterpret_cast<Udata *>(event.udata);
}

void Webserv::RunServer() {
	while (true) {
		std::vector<struct kevent> event_list = kq_handler_.MonitorEvent(); // get event
		for (size_t i = 0; i < event_list.size(); ++i) {
			struct kevent event = event_list[i];
			if (IsProcessExitEvent(event)) {
				DeleteProcEvent(event);
				continue;
			}
			if (IsDisconnected(event)) {
				if (IsEventHasData(event)) { // Half-Close
					ClientSocket *client_socket = FindClientSocket((int) event.ident);
					client_socket->SetHalfClose();
					EventExecutor::ReceiveRequest(kq_handler_, event);
					continue;
				}
				DeleteClient(event);
				continue;
			}
			if (IsLogEvent(event)) {  // write log
				WriteLog(event);
				continue;
			}
			HandleEvent(event);
		}
	}
}

void Webserv::WriteLog(struct kevent &event) {
	Logger *logger = reinterpret_cast<Logger *>(event.udata);
	logger->WriteLog(event);
	delete logger;
}

void Webserv::HandleEvent(struct kevent &event) {
	int state = reinterpret_cast<Udata *>(event.udata)->GetState();

	try {
		switch (state) {
			case Udata::LISTEN: EventExecutor::AcceptClient(kq_handler_, event);
				return;
			case Udata::RECV_REQUEST: EventExecutor::ReceiveRequest(kq_handler_, event);
				break;
			case Udata::READ_FILE: EventExecutor::ReadFile(kq_handler_, event);
				break;
			case Udata::WRITE_FILE: EventExecutor::WriteFile(kq_handler_, event);
				break;
			case Udata::CGI_PIPE: HandleCgi(event);
				break;
			case Udata::SEND_RESPONSE: EventExecutor::SendResponse(kq_handler_, event);
				break;
		}
	} catch (const HttpException &e) {
		HandleException(e, event);
	} catch (...) { // TODO: Remove
		std::perror("Unknown:");
		exit(1);
	}
}

void Webserv::HandleCgi(struct kevent &event) {
	if (IsReadEvent(event)) {
		EventExecutor::ReadCgiResultFromPipe(kq_handler_, event);
		return;
	}
	EventExecutor::WriteReqBodyToPipe(event);
}

void Webserv::HandleException(const HttpException &e, struct kevent &event) {
	kq_handler_.AddWriteLogEvent(Webserv::error_log_fd_, new Logger(e.what())); // make error logs

	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	kq_handler_.DeleteEvent(event);
	ResponseMessage response_message(e.GetStatusCode(), e.GetReasonPhrase()); // make response message
	if (user_data->request_message_.ShouldClose())
		response_message.AddConnection("close");
	user_data->response_message_ = response_message;
	user_data->ChangeState(Udata::SEND_RESPONSE);
	kq_handler_.AddWriteEvent(user_data->sock_d_, user_data); // send response
}
