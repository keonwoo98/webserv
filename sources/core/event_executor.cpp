#include <sstream>
#include <fcntl.h>

#include "html.hpp"
#include "event_executor.hpp"
#include "client_socket.hpp"
#include "request_message.hpp"
#include "request_parser.hpp"
#include "request_validation.hpp"
#include "response_message.hpp"
#include "resolve_uri.hpp"
#include "http_exception.hpp"
#include "udata.hpp"
#include "webserv.hpp"
#include "logger.hpp"
#include "cgi_handler.hpp"
#include "file_util.hpp"
#include "cgi_parser.hpp"
#include "auto_index.hpp"

void EventExecutor::AcceptClient(KqueueHandler &kqueue_handler, const struct kevent &event) {
	ServerSocket *server_socket = Webserv::FindServerSocket((int) event.ident);
	ClientSocket *client_socket = server_socket->AcceptClient();

	if (client_socket == NULL) {
		std::string log_msg = Logger::MakeAcceptFailLog(server_socket); // Make Accept Failed Log
		kqueue_handler.AddWriteLogEvent(Webserv::error_log_fd_, new Logger(log_msg));
		return;
	}

	std::string log_msg = Logger::MakeAcceptLog(client_socket); // Make Access Log
	kqueue_handler.AddWriteLogEvent(Webserv::access_log_fd_, new Logger(log_msg));

	// Add RECV_REQUEST Event
	int sock_d = client_socket->GetSocketDescriptor();
	kqueue_handler.AddReadEvent(sock_d, new Udata(Udata::RECV_REQUEST, sock_d));
	Webserv::clients_.insert(std::make_pair(sock_d, client_socket));
}

void EventExecutor::ReceiveRequest(KqueueHandler &kqueue_handler, const struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	RequestMessage &request = user_data->request_message_;
	ClientSocket *client_socket = Webserv::FindClientSocket((int) event.ident);
	int client_sock_d = client_socket->GetSocketDescriptor();

	char buf[BUFSIZ];
	ssize_t result = recv(client_sock_d, buf, BUFSIZ, 0);
	if (result <= 0) {
		throw HttpException(INTERNAL_SERVER_ERROR, "(Receive Request) : recv errror");
	}

	const ServerSocket *server_socket = Webserv::FindServerSocket(client_socket->GetServerFd());
	const ConfigParser::server_infos_type &server_infos = server_socket->GetServerInfos();
	ParseRequest(request, buf, result);

	if (request.GetState() == DONE) {
		if (client_socket->IsHalfClose()) { // Half Close
			shutdown(event.ident, SHUT_RD);
		}
		kqueue_handler.DeleteEvent(event);
		CheckRequest(request, client_socket, server_infos);
		kqueue_handler.AddWriteLogEvent(Webserv::access_log_fd_,
										new Logger(Logger::MakeRequestLog(request))); // make access log (request message)
		ResponseMessage &response = user_data->response_message_;
		if (request.ShouldClose()) {
			response.AddConnection("close");
		}
		if (client_socket->GetServerInfo().IsRedirect()) {
			response.SetStatusLine(TEMPORARY_REDIRECT, "redirection");
			response.AddLocation(client_socket->GetServerInfo().GetRedirect());
			response.SetContentLength();
			user_data->ChangeState(Udata::SEND_RESPONSE);
			kqueue_handler.DeleteEvent(event);
			kqueue_handler.AddWriteEvent(event.ident, user_data);
			return;
		}
		HandleRequestResult(client_socket, user_data, kqueue_handler);
	}
}

/**
 * DELETE Method
 *
 * Directory 삭제 시도 -> 409 Conflict
 * 없는 파일 삭제 시도 -> 404 Not Found
 * 성공 -> 200 OK
 */
ResponseMessage EventExecutor::HandleDeleteMethod(const std::string &uri, ResponseMessage &response_message) {
	int fd = open(uri.c_str(), O_RDWR);
	if (fd < 0) {
		throw HttpException(NOT_FOUND, std::strerror(errno));
	}
	if (unlink(uri.c_str()) < 0) {
		close(fd); // DELETE FAILED
		throw HttpException(FORBIDDEN, std::strerror(errno));
	}
	close(fd);
	response_message.SetStatusLine(OK, "OK");
	response_message.AppendBody(delete_ok_html.c_str());
	return response_message;
}

int EventExecutor::HandlePutMethod(Udata *user_data, const std::string &resolved_uri) {
	user_data->request_message_.SetResolvedUri(resolved_uri);
	int fd = open(resolved_uri.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
	if (fd < 0) {
		close(fd);
		throw HttpException(INTERNAL_SERVER_ERROR, "(HandleRequestResult) : open fails");
	}
	fcntl(fd, F_SETFL, O_NONBLOCK);
	return fd;
}

void EventExecutor::HandleAutoIndex(KqueueHandler &kqueue_handler, Udata *user_data, const std::string resolved_uri) {
	std::string auto_index = AutoIndexHtml(user_data->request_message_.GetUri(), MakeDirList(resolved_uri));
	user_data->response_message_.SetStatusLine(OK, "OK");
	user_data->response_message_.AppendBody(auto_index.c_str());
	user_data->ChangeState(Udata::SEND_RESPONSE);
	kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
}

/**
 * DELETE Method
 *
 * Directory 삭제 시도 -> 409 Conflict
 * 없는 파일 삭제 시도 -> 404 Not Found
 * 성공 -> 200 OK
 */

void EventExecutor::HandleRequestResult(ClientSocket *client_socket, Udata *user_data, KqueueHandler &kqueue_handler) {
	ResolveURI r_uri(client_socket->GetServerInfo(), user_data->request_message_);

	ServerInfo server_info = client_socket->GetServerInfo();
	const std::string &method = user_data->request_message_.GetMethod();
	user_data->request_message_.SetResolvedUri(r_uri.GetResolvedUri());

	if (method == "DELETE") {
		// delete method run -> check auto index (if on then throw not allow method status code)
		HandleDeleteMethod(r_uri.GetResolvedUri(), user_data->response_message_);
		user_data->ChangeState(Udata::SEND_RESPONSE);
		kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
	} else if (r_uri.ResolveCGI()) { // CGI (GET / POST)
		user_data->request_message_.SetResolvedUri(r_uri.GetResolvedUri());
		CgiHandler cgi_handler(r_uri.GetCgiPath());
		cgi_handler.SetupAndAddEvent(kqueue_handler, user_data, client_socket, server_info);
	} else if (method == "GET" || method == "POST" || method == "HEAD") {
		if (r_uri.ResolveIndex()) {
			user_data->request_message_.SetResolvedUri(r_uri.GetResolvedUri());
			HandleAutoIndex(kqueue_handler, user_data, r_uri.GetResolvedUri());
			return;
		}
		user_data->request_message_.SetResolvedUri(r_uri.GetResolvedUri());
		HandleStaticFile(kqueue_handler, user_data);
	} else if (method == "PUT") {
		int fd = HandlePutMethod(user_data, r_uri.GetResolvedUri());
		user_data->ChangeState(Udata::WRITE_FILE);
		kqueue_handler.AddWriteEvent(fd, user_data);
	} else {
		throw HttpException(INTERNAL_SERVER_ERROR, "Handle Request Result Unknown Error");
	}
}

void EventExecutor::HandleStaticFile(KqueueHandler &kqueue_handler, Udata *user_data) {
	std::string resolve_uri = user_data->request_message_.GetResolvedUri();
	int fd = open(resolve_uri.c_str(), O_RDONLY);
	CheckStaticFileOpenError(fd);
	long file_size = GetFileSize(resolve_uri.c_str());
	if (file_size > 0) {
		user_data->ChangeState(Udata::READ_FILE);
		kqueue_handler.AddReadEvent(fd, user_data);
	} else if (file_size == 0) {
		close(fd);
		user_data->ChangeState(Udata::SEND_RESPONSE);
		user_data->response_message_.SetStatusLine(OK, "OK");
		user_data->response_message_.AppendBody("");
		kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
	} else {
		close(fd);
		throw HttpException(INTERNAL_SERVER_ERROR, std::strerror(errno));
	}
}

/**
 * TODO: kqueue_handler 사용 변경
 * @param fd
 * @param readable_size
 * @param response_message
 * @return
 */
void EventExecutor::ReadFile(KqueueHandler &kqueue_handler, struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	ResponseMessage &response_message = user_data->response_message_;
	char buf[ResponseMessage::BUFFER_SIZE];
	ssize_t result = read(event.ident, buf, ResponseMessage::BUFFER_SIZE);
	if (result <= 0) {
		close(event.ident);
		throw HttpException(INTERNAL_SERVER_ERROR, "Read File read()");
	}
	response_message.AppendBody(buf, result);
	if (result < event.data) {
		return;
	}
	if (!response_message.IsStatusExist()) {
		response_message.SetStatusLine(OK, "OK");
	}
	close(event.ident);
	user_data->ChangeState(Udata::SEND_RESPONSE);
	kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
}

void EventExecutor::WriteFile(KqueueHandler &kqueue_handler, struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	RequestMessage &request_message = user_data->request_message_;
	const std::string &body = request_message.GetBody();

	ssize_t result =
		write(event.ident, body.c_str() + request_message.current_length_,
			  body.length() - request_message.current_length_);

	if (result <= 0) {
		close(event.ident);
		throw HttpException(INTERNAL_SERVER_ERROR, "WriteFile() write: ");
	}

	request_message.current_length_ += result;
	if (request_message.current_length_ >= body.length()) {
		close(event.ident);
		user_data->response_message_.SetContentLength();
		user_data->response_message_.SetStatusLine(CREATED, "Created");
		user_data->ChangeState(Udata::SEND_RESPONSE);
		kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
	}
}

void EventExecutor::WriteReqBodyToPipe(KqueueHandler &kqueue_handler,
									   struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	RequestMessage &request_message = user_data->request_message_;
	const std::string &body = request_message.GetBody();

	if (body.length() == 0) {
		close(event.ident);
		return;
	}
	ssize_t result = write(event.ident, body.c_str() + request_message.current_length_,
						   body.length() - request_message.current_length_);
	if (result <= 0) {
		kqueue_handler.AddWriteLogEvent(
			Webserv::error_log_fd_,
			new Logger("(WriteReqBodyToPipe) : write error\n"));
		close(event.ident);
		return;
	}
	request_message.current_length_ += result;
	if (request_message.current_length_ >= body.length()) {
		close(event.ident);
	}
	// AddEvent는 이미 SetupCgi에서 해주었었기 때문에 할 필요가 없다. ChangeState만 해주면 됨
}

void EventExecutor::ReadCgiResultFromPipe(KqueueHandler &kqueue_handler,
										  struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	ResponseMessage &response_message = user_data->response_message_;
	char buf[ResponseMessage::BUFFER_SIZE];

	ssize_t result = read(event.ident, buf, ResponseMessage::BUFFER_SIZE);
	if (result < 0) {
		close(event.ident);
		throw HttpException(INTERNAL_SERVER_ERROR, "(ReadCgiResultFromPipe) : read error");
	}
	if (result == 0) {
		close(event.ident);
		ParseCgiResult(response_message);
		if (!response_message.IsStatusExist())
			response_message.SetStatusLine(OK, "OK"); // TODO: Parse Status Code
		response_message.SetContentLength();
		user_data->ChangeState(Udata::SEND_RESPONSE);
		kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
		return;
	}
	response_message.AppendBody(buf, result);
}

/**
 * - Body가 비어있는 경우
 * 	- Error Page가 설정되어 있는 경우 -> READ
 * 	- Error Page가 없는 경우 -> Default error page
 * - Body가 이미 있는 경우 -> return
 * @param kqueue_handler
 * @param client_socket
 * @param user_data
 */
int EventExecutor::CheckErrorPages(ClientSocket *client_socket, Udata *user_data) {
	ResponseMessage &response = user_data->response_message_;

	std::string error_page_path = response.GetErrorPagePath(client_socket->GetServerInfo());
	if (response.BodySize() <= 0) {
		if (error_page_path.length() > 0) {
			int error_page_fd = open(error_page_path.c_str(), O_RDONLY);
			if (error_page_fd > 0) {
				fcntl(error_page_fd, F_SETFL, O_NONBLOCK);
				return error_page_fd;
			}
		}
		response.AppendBody(default_error_html.c_str()); // default error page
	}
	return -1;
}

ssize_t DecideDataLength(int fd, const ResponseMessage &response_message) {
	ssize_t send_len = Socket::GetSendBufferSize(fd);
	ssize_t remain = response_message.total_length_ - response_message.current_length_;
	if (send_len >= remain) {
		return remain;
	}
	return send_len;
}

void CloseConnection(Udata *user_data, ClientSocket *client_socket) {
	user_data->Reset();
	delete user_data;
	Webserv::clients_.erase(client_socket->GetSocketDescriptor());
	delete client_socket;
}

void EventExecutor::SendResponse(KqueueHandler &kqueue_handler, struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	ClientSocket *client_socket = Webserv::FindClientSocket(user_data->sock_d_);
	int fd = client_socket->GetSocketDescriptor();
	RequestMessage &request = user_data->request_message_;
	request.GetBody().clear(); // TODO: 적절한 위치로 변경
	ResponseMessage &response = user_data->response_message_;
	const std::string &method = request.GetMethod();

	if (method != "HEAD" && response.IsErrorStatus()) {
		int error_page_fd = CheckErrorPages(client_socket, user_data);
		if (error_page_fd > 0) {
			kqueue_handler.DeleteEvent(event); // DELETE SEND_RESPONSE
			user_data->ChangeState(Udata::READ_FILE);
			kqueue_handler.AddReadEvent(error_page_fd, user_data); // ADD READ_FILE
			return;
		}
	}
	if (method == "HEAD") {
		response.ClearBody();
	}
	const std::string &response_str = response.ToString();
	ssize_t to_send = DecideDataLength(fd, response);
	ssize_t result = send(fd, response_str.c_str(), to_send, 0);
	if (result <= 0) {
		kqueue_handler.DeleteEvent(event);
		kqueue_handler.AddWriteEvent(event.ident, event.udata);
		return; // try again
	}
	response.AddCurrentLength(result);
	if (response.IsDone()) {
		if (request.ShouldClose() || client_socket->IsHalfClose()) {    // connection: close
			CloseConnection(user_data, client_socket);
			return;
		}
		kqueue_handler.DeleteEvent(event); // keep-alive
		user_data->Reset();    // reset user data (state = RECV_REQUEST)
		kqueue_handler.AddReadEvent(fd, user_data);    // RECV_REQUEST
	}
}
