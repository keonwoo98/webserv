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
#include "fd_handler.hpp"
#include "cgi_parser.hpp"
#include "auto_index.hpp"

void EventExecutor::AcceptClient(KqueueHandler &kqueue_handler, struct kevent &event) {
	ServerSocket *server_socket = Webserv::FindServerSocket(event.ident);
	ClientSocket *client_socket = server_socket->AcceptClient();

	if (client_socket == NULL) { // Make Accept Failed Log
		std::stringstream ss;
		ss << server_socket << '\n' << "Accept Failed" << std::endl;
		kqueue_handler.AddWriteLogEvent(Webserv::error_log_fd_, new Logger(ss.str()));
		return;
	}

	// Make Access Log
	std::stringstream ss;
	ss << "New Client Accepted\n" << client_socket << std::endl;
	kqueue_handler.AddWriteLogEvent(Webserv::access_log_fd_, new Logger(ss.str()));


	int sock_d = client_socket->GetSocketDescriptor();
	// Add RECV_REQUEST Event
	Udata *udata = new Udata(Udata::RECV_REQUEST, sock_d);
	kqueue_handler.AddReadEvent(sock_d, udata); // client RECV_REQUEST
	Webserv::clients_.insert(std::make_pair(sock_d, client_socket));
}

/*
 * Request Message에 resolved uri가 있는 경우
 * */
void EventExecutor::ReceiveRequest(KqueueHandler &kqueue_handler, const struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	RequestMessage &request = user_data->request_message_;
	ClientSocket *client_socket = Webserv::FindClientSocket(event.ident);

	char buf[BUFSIZ];
	ssize_t recv_len = recv(client_socket->GetSocketDescriptor(), buf, BUFSIZ, 0);
	if (recv_len < 0) {
		throw HttpException(INTERNAL_SERVER_ERROR, "(Receive Request) : recv errror");
	}

	const ServerSocket *server_socket = Webserv::FindServerSocket(client_socket->GetServerFd());
	const ConfigParser::server_infos_type &server_infos = server_socket->GetServerInfos();
	ParseRequest(request, client_socket, server_infos, buf, recv_len);

	if (request.GetState() == DONE) {
		kqueue_handler.DeleteEvent(event);
		CheckRequest(request, client_socket, server_infos);
		// make access log (request message)
		std::stringstream ss;
		ss << request << std::endl;
		kqueue_handler.AddWriteLogEvent(Webserv::access_log_fd_, new Logger(ss.str()));

		ResponseMessage &response = user_data->response_message_;
		if (request.ShouldClose()) {
			response.AddConnection("close");
		}
		if (client_socket->GetServerInfo().IsRedirect()) {
			// redirect uri 를 response header에 추가해줘야함.
			// TODO: 왜 redirect도 exception으로..?
			throw HttpException(TEMPORARY_REDIRECT, "redirect");
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
ResponseMessage DeleteMethod(const std::string &uri, ResponseMessage &response_message) {
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

void EventExecutor::HandleAutoIndex(KqueueHandler &kqueue_handler, Udata *user_data, const std::string resolved_uri) {
	std::string auto_index = AutoIndexHtml(user_data->request_message_.GetUri(), MakeDirList(resolved_uri));
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
	// if (allowed method가 아닌 경우)
	// 405 Method Not Allowed
	user_data->request_message_.SetResolvedUri(r_uri.GetResolvedUri());

	if (method == "DELETE") {
		// delete method run -> check auto index (if on then throw not allow method status code)
		DeleteMethod(r_uri.GetResolvedUri(), user_data->response_message_);
		user_data->ChangeState(Udata::SEND_RESPONSE);
		kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
	} else if (r_uri.ResolveCGI()) { // CGI (GET / POST)
		user_data->request_message_.SetResolvedUri(r_uri.GetResolvedUri());
		CgiHandler cgi_handler(r_uri.GetCgiPath());
		cgi_handler.SetupAndAddEvent(kqueue_handler, user_data, client_socket, server_info);
	} else if (method == "GET" || method == "POST" || method == "HEAD") {
		if ((method == "GET" || method == "HEAD") && r_uri.ResolveIndex()) {
			user_data->request_message_.SetResolvedUri(r_uri.GetResolvedUri());
			HandleAutoIndex(kqueue_handler, user_data, r_uri.GetResolvedUri());
			return;
		}
		user_data->request_message_.SetResolvedUri(r_uri.GetResolvedUri());
		HandleStaticFile(kqueue_handler, user_data);
	} else if (method == "PUT") {
		user_data->request_message_.SetResolvedUri(r_uri.GetResolvedUri());
		int fd = open(r_uri.GetResolvedUri().c_str(),
					  O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
		if (fd < 0) {
			close (fd);
			throw HttpException(INTERNAL_SERVER_ERROR, "(HandleRequestResult) : open fails");
		}
		fcntl(fd, F_SETFL, O_NONBLOCK);
		user_data->ChangeState(Udata::WRITE_FILE);
		kqueue_handler.AddWriteEvent(fd, user_data);
	} else {
		throw HttpException(INTERNAL_SERVER_ERROR, "Handle Request Result Unknown Error");
	}
}

void EventExecutor::HandleStaticFile(KqueueHandler &kqueue_handler, Udata *user_data) {
	std::string resolve_uri = user_data->request_message_.GetResolvedUri();
	int fd = OpenFile(resolve_uri.c_str());
	if (fd < 0) {
		if (errno == ENOENT) {
			throw HttpException(NOT_FOUND, std::strerror(errno));
		}
		if (errno == EACCES) {
			throw HttpException(FORBIDDEN, std::strerror(errno));
		}
		throw HttpException(NOT_ACCEPTABLE, "test throw");
	}
	long file_size = GetFileSize(resolve_uri.c_str());
	if (file_size > 0) {
		user_data->ChangeState(Udata::READ_FILE);
		kqueue_handler.AddReadEvent(fd, user_data);
	} else if (file_size == 0) {
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
	ssize_t size = read(event.ident, buf, ResponseMessage::BUFFER_SIZE);
	if (size < 0) {
		close(event.ident);
		throw HttpException(INTERNAL_SERVER_ERROR, "Read File read()");
	}
	response_message.AppendBody(buf, size);
	if (size < event.data) {
		return;
	}
	// TODO: 파일을 다 읽었다는 것을 어떻게 알 수 있는가?
	response_message.SetStatusLine(OK, "OK");
	close(event.ident);
	user_data->ChangeState(Udata::SEND_RESPONSE);
	kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
}

void EventExecutor::WriteFile(KqueueHandler &kqueue_handler, struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	RequestMessage &request_message = user_data->request_message_;
	std::string body = request_message.GetBody();

	ssize_t result =
			write(event.ident, body.c_str() + request_message.current_length_,
				  body.length() - request_message.current_length_);

	if (result < 0) {
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

void EventExecutor::WriteReqBodyToPipe(struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	RequestMessage &request_message = user_data->request_message_;
	std::string body = request_message.GetBody();

	ssize_t result = write(event.ident, body.c_str() + request_message.current_length_,
						   body.length() - request_message.current_length_);
	if (result < 0) {
		close(event.ident);
		// TODO: kqueue handler로 error log 작성
//		throw HttpException(INTERNAL_SERVER_ERROR, "WriteReqBodyToPipe write()");
	}
	request_message.current_length_ += result;
	if (request_message.current_length_ >= body.length()) {
		close(event.ident);
		user_data->ChangeState(Udata::READ_FROM_PIPE);
	}
	// AddEvent는 이미 SetupCgi에서 해주었었기 때문에 할 필요가 없다. ChangeState만 해주면 됨
}

void EventExecutor::ReadCgiResultFromPipe(KqueueHandler &kqueue_handler,
										  struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	ResponseMessage &response_message = user_data->response_message_;
	char buf[ResponseMessage::BUFFER_SIZE];

	ssize_t size = read(event.ident, buf, ResponseMessage::BUFFER_SIZE);
	if (size < 0) {
		close(event.ident);
		throw HttpException(INTERNAL_SERVER_ERROR, "ReadCgiResultFromPipe read()");
	}
	if (size == 0) {
		close(event.ident);
		ParseCgiResult(response_message);
		response_message.SetStatusLine(OK, "OK"); // TODO: Parse Status Code
		response_message.SetContentLength();
		user_data->ChangeState(Udata::SEND_RESPONSE);
		kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
		return;
	}
	response_message.AppendBody(buf, size);
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

/**
 * Response Message에 필요한 header, body가 이미 설정되었다고 가정
 * TODO: chunked response message
 */
void EventExecutor::SendResponse(KqueueHandler &kqueue_handler, struct kevent &event) {
	Udata *user_data = reinterpret_cast<Udata *>(event.udata);
	ClientSocket *client_socket = Webserv::FindClientSocket(user_data->sock_d_);
	int fd = client_socket->GetSocketDescriptor();
	RequestMessage &request = user_data->request_message_;
	ResponseMessage &response = user_data->response_message_;
	std::string method = request.GetMethod();

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
	std::string response_str = response.ToString();
	ssize_t send_len = send(fd,
							response_str.c_str() + response.current_length_,
							response_str.length() - response.current_length_, 0);
	if (send_len < 0) {
		throw HttpException(INTERNAL_SERVER_ERROR, "send response send() error");
	}
	response.AddCurrentLength(send_len);
	// std::cout << response_str.c_str() << std::endl;
	// std::cout << send_len << std::endl;
	// std::cout << response_str.c_str() << std::endl;
	if (response.IsDone()) {
		kqueue_handler.AddWriteLogEvent(Webserv::access_log_fd_, new Logger(response.ToString()));
		if (request.ShouldClose()) {    // connection: close
			delete user_data;
			Webserv::clients_.erase(fd);
			delete client_socket;
			return;
		}
		kqueue_handler.DeleteEvent(event);
		user_data->Reset();    // reset user data (state = RECV_REQUEST)
		kqueue_handler.AddReadEvent(fd, user_data);    // RECV_REQUEST
		request.total_length_ = 0;
		request.current_length_ = 0;
	}
}
