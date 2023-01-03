#include <sstream>

#include "webserv.hpp"
#include "event_executor.hpp"
#include "udata.hpp"
#include "core_exception.hpp"
#include "logger.hpp"

// open log files
int Webserv::access_log_fd_ = open("./logs/access.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
int Webserv::error_log_fd_ = open("./logs/error.log", O_WRONLY | O_CREAT | O_APPEND, 0644);

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
ServerSocket *Webserv::FindServerSocket(int fd) const {
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
ClientSocket *Webserv::FindClientSocket(int fd) const {
	Webserv::clients_type::const_iterator it = clients_.find(fd);
	if (it == clients_.end()) {
		return NULL;
	}
	return clients_.find(fd)->second;
}

void Webserv::WaitChildProcess(int pid) const {
	int status;
	if (waitpid(pid, &status, WNOHANG) < 0 ||
		!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
		// waitpid failed or execve failed or cgi return error status
		throw CoreException::CgiExecutionException();
	}
}

void Webserv::DeleteClient(const struct kevent &event) {
	ClientSocket *client_socket = FindClientSocket(event.ident);
	if (client_socket) {
		delete client_socket;
		clients_.erase(client_socket->GetSocketDescriptor());
	}
}

bool Webserv::IsProcessExit(const struct kevent &event) const {
	return event.fflags & NOTE_EXIT;
}

/**
* EV_EOF
* - If the read direction of the socket has shutdown (socket)
* - When the last writer disconnects (pipe)
*/
bool Webserv::IsDisconnected(const struct kevent &event) const {
	return ((event.flags & EV_EOF) && FindClientSocket(event.ident));
}

bool Webserv::IsLogEvent(const struct kevent &event) const {
	return (int)event.ident == error_log_fd_ || (int)event.ident == access_log_fd_;
}

void Webserv::RunServer() {
	while (true) {
		struct kevent event = kq_handler_.MonitorEvent(); // get event
		if (IsProcessExit(event)) {
			WaitChildProcess(event.ident);
			continue;
		}
		if (IsDisconnected(event)) {
			DeleteClient(event);
			delete reinterpret_cast<Udata *>(event.udata);
			continue;
		}
		if (IsLogEvent(event)) { // write log
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


void Webserv::HandleException(const HttpException &e, struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	kq_handler_.AddWriteLogEvent(Webserv::error_log_fd_, new Logger(e.what())); // make error log

	ResponseMessage response_message(e.GetStatusCode(), e.GetReasonPhrase()); // make response message
	if (user_data->request_message_.ShouldClose())
		response_message.AddConnection("close");
	user_data->response_message_ = response_message;
	user_data->ChangeState(Udata::SEND_RESPONSE);
	kq_handler_.AddWriteEvent(user_data->sock_d_, user_data); // send response
}

void Webserv::HandleEvent(struct kevent &event) {
	int state = reinterpret_cast<Udata *>(event.udata)->GetState();

	try {
		switch (state) {
			case Udata::LISTEN:
				HandleListenEvent(event);
				return;
			case Udata::RECV_REQUEST:
				HandleReceiveRequestEvent(event);
				break;
			case Udata::READ_FILE:
				HandleReadFile(event);
				break;	// GET
			case Udata::WRITE_TO_PIPE:
				HandleWriteToPipe(event);
				break;	// CGI
			case Udata::READ_FROM_PIPE:
				HandleReadFromPipe(event);
				break;	// CGI
			case Udata::SEND_RESPONSE:
				HandleSendResponseEvent(event);
				break;
		}
	} catch (const HttpException &e) {
		HandleException(e, event);
	} catch (...) { // for debugging
		std::cerr << "Unknown Error" << std::endl;
		exit(1);
	}
}

void Webserv::HandleListenEvent(struct kevent &event) {
	ServerSocket *server_socket = FindServerSocket(event.ident);
	ClientSocket *client_socket = EventExecutor::AcceptClient(kq_handler_, server_socket); // accept client
	if (client_socket == NULL) {
		return;
	}
	clients_.insert(std::make_pair(client_socket->GetSocketDescriptor(), client_socket)); // insert client to clients map
}

void Webserv::HandleReceiveRequestEvent(struct kevent &event) {
	ClientSocket *client_socket = FindClientSocket(event.ident);
	ServerSocket *server_socket = FindServerSocket(client_socket->GetServerFd());
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	EventExecutor::ReceiveRequest(kq_handler_, client_socket, server_socket, user_data);
}

void Webserv::HandleReadFile(struct kevent &event) {
	int file_to_read = event.ident;
	int readable_size = event.data;
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);

	EventExecutor::ReadFile(kq_handler_, file_to_read, readable_size, user_data);
}

void Webserv::HandleWriteToPipe(struct kevent &event) {
	EventExecutor::WriteReqBodyToPipe(event);
}

void Webserv::HandleReadFromPipe(struct kevent &event) {
	EventExecutor::ReadCgiResultFromPipe(kq_handler_, event);
}

void Webserv::HandleSendResponseEvent(struct kevent &event) {
	ClientSocket *client_socket = FindClientSocket(event.ident);
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);

	try {
		int result = EventExecutor::SendResponse(kq_handler_, client_socket, user_data);
		if (result == Udata::CLOSE) {
			delete user_data;
			user_data = NULL;
		}
	} catch (const HttpException &e) { // error log
		kq_handler_.AddWriteLogEvent(error_log_fd_, new Logger(e.what()));
		delete user_data;
		user_data = NULL;
	}
	if (user_data == NULL) {
		clients_.erase(client_socket->GetSocketDescriptor()); // delete client socket from clients map
		delete client_socket; // deallocate client socket (socket closed)
	}
}
