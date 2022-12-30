#include <sstream>
#include <fcntl.h>
#include <dirent.h>

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

ClientSocket *EventExecutor::AcceptClient(KqueueHandler &kqueue_handler, ServerSocket *server_socket) {
	ClientSocket *client_socket = server_socket->AcceptClient();
	if (client_socket == NULL) { // Make Accept Failed Log
		std::stringstream ss;
		ss << server_socket << '\n' << "Accept Failed" << std::endl;
		kqueue_handler.AddWriteOnceEvent(Webserv::error_log_fd_, new Logger(ss.str()));
		return NULL;
	}
	int sock_d = client_socket->GetSocketDescriptor();

	// Make Access Log
	std::stringstream ss;
	ss << "New Client Accepted\n" << client_socket << std::endl;
	kqueue_handler.AddWriteOnceEvent(Webserv::access_log_fd_, new Logger(ss.str()));

	// Add RECV_REQUEST Event
	Udata *udata = new Udata(Udata::RECV_REQUEST, sock_d);
	kqueue_handler.AddReadEvent(sock_d, udata); // client RECV_REQUEST
	return client_socket;
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
		if (errno == ENOENT) {
			response_message.SetStatusLine(NOT_FOUND, "NOT FOUND");
			return response_message;
		}
		response_message.SetStatusLine(CONFLICT, "CONFLICT");
		return response_message;
	}
	if (unlink(uri.c_str()) < 0) {
		close(fd); // DELETE FAILED
		response_message.SetStatusLine(CONFLICT, "CONFLICT");
		return response_message;
	}
	close(fd);
	response_message.SetStatusLine(OK, "OK");
	response_message.AppendBody(delete_ok_html.c_str());
	return response_message;
}

std::string MakeAutoindexHtml(const std::string &dir_uri) {
	// Open the directory
	DIR *dir = opendir(dir_uri.c_str());
	if (!dir) {
		throw HttpException(NOT_FOUND, "Error: Could not open directory");
	}

	std::stringstream ss;
	// Write the HTML header
	ss << auto_index_prefix;
	// Iterate through the directory entries
	struct dirent *entry;
	while ((entry = readdir(dir)) != nullptr) {
		// Skip hidden files
		if (entry->d_name[0] == '.')
			continue;
		// Write a list item for the file
		ss << "		<li><a href=\"" << entry->d_name << "\">" << entry->d_name << "</a></li>\n";
	}
	// Write the HTML footer
	ss << auto_index_suffix;
	closedir(dir);
	return ss.str();
}

void EventExecutor::HandleAutoIndex(KqueueHandler &kqueue_handler, Udata *user_data, const std::string resolved_uri) {
	std::string auto_index = MakeAutoindexHtml(resolved_uri);
	user_data->response_message_.AppendBody(auto_index.c_str());
	user_data->ChangeState(Udata::SEND_RESPONSE);
	kqueue_handler.DeleteReadEvent(user_data->sock_d_);
	kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
}

void EventExecutor::HandleRequestResult(ClientSocket *client_socket, Udata *user_data, KqueueHandler &kqueue_handler) {
	ResolveURI r_uri(client_socket->GetServerInfo(), user_data->request_message_);

	ServerInfo server_info = client_socket->GetServerInfo();
	const std::string &method = user_data->request_message_.GetMethod();
	// if (allowed method가 아닌 경우)
	// 405 Method Not Allowed

	if (method == "DELETE") {
		// delete method run -> check auto index (if on then throw not allow method status code)
		DeleteMethod(r_uri.GetResolvedUri(), user_data->response_message_);
		user_data->ChangeState(Udata::SEND_RESPONSE);
		kqueue_handler.DeleteReadEvent(user_data->sock_d_);
		kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
	} else if (r_uri.IsCgi()) { // CGI
		// CGI handler execute;
		CgiHandler cgi_handler(r_uri.GetCgiPath());
		cgi_handler.SetupAndAddEvent(kqueue_handler, user_data, client_socket);
	} else if (method == "GET") { // GET
		if (r_uri.IsAutoIndex()) { // Auto Index
			HandleAutoIndex(kqueue_handler, user_data, r_uri.GetResolvedUri());
			return;
		}
		user_data->ChangeState(Udata::READ_FILE);
		kqueue_handler.DeleteReadEvent(user_data->sock_d_);
		kqueue_handler.AddReadEvent(OpenFile(user_data), user_data);
	} else if (method == "POST") { // POST
	} else {
		throw (HttpException(INTERNAL_SERVER_ERROR, "unknown error"));
	}
}

/*
 * Request Message에 resolved uri가 있는 경우
 * TODO: kqueue_handler 사용하도록 변경
 * */
void EventExecutor::ReceiveRequest(KqueueHandler &kqueue_handler,
								   ClientSocket *client_socket,
								   const ServerSocket *server_socket,
								   Udata *user_data) {
	ResponseMessage &response = user_data->response_message_;
	RequestMessage &request = user_data->request_message_;

	char buf[BUFSIZ + 1];
	int recv_len = recv(client_socket->GetSocketDescriptor(),
						buf, BUFSIZ, 0);
	if (recv_len < 0) {
		throw HttpException(INTERNAL_SERVER_ERROR, "(event_executor) : recv errror");
	}
	buf[recv_len] = '\0';
	const ConfigParser::server_infos_type &server_infos = server_socket->GetServerInfos();
	ParseRequest(request, client_socket, server_infos, buf);
	if (request.GetState() == DONE) {
		// make access log (request message)
		std::stringstream ss;
		ss << request << std::endl;
		kqueue_handler.AddWriteOnceEvent(Webserv::access_log_fd_, new Logger(ss.str()));
		if (request.ShouldClose())
			response.AddConnection("close");
		if (client_socket->GetServerInfo().IsRedirect()) {
			// redirect uri 를 response header에 추가해줘야함.
			throw (HttpException(TEMPORARY_REDIRECT, "redirect"));
		}
		HandleRequestResult(client_socket, user_data, kqueue_handler);
	}
}

/**
 * TODO: kqueue_handler 사용 변경
 * @param fd
 * @param readable_size
 * @param response_message
 * @return
 */
void EventExecutor::ReadFile(KqueueHandler &kqueue_handler, int fd,
							 int readable_size, Udata *user_data) {
	char buf[ResponseMessage::BUFFER_SIZE + 1];
	ResponseMessage &response_message = user_data->response_message_;
	ssize_t size = read(fd, buf, ResponseMessage::BUFFER_SIZE);
	buf[size] = '\0';
	if (size < 0) {
		throw HttpException(500, "Read File read()");
	}
	response_message.AppendBody(buf, size);
	if (size < readable_size) {
		return;
	}
	// TODO: 파일을 다 읽었다는 것을 어떻게 알 수 있는가?
	response_message.SetStatusLine(OK, "OK");
	close(fd);
	user_data->ChangeState(Udata::SEND_RESPONSE);
	kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
}

void EventExecutor::WriteReqBodyToPipe(const int &fd, Udata *user_data) {
	const RequestMessage &request_message = user_data->request_message_;
	std::string body = request_message.GetBody();
	char *body_c_str = new char[body.length() + 1];
	std::strcpy(body_c_str, body.c_str());

	ssize_t result = write(fd, body_c_str, body.length() + 1);
	if (result < 0) {
		std::perror("write: ");
	}
	close(fd);
	user_data->ChangeState(Udata::READ_FROM_PIPE);
	// AddEvent는 이미 SetupCgi에서 해주었었기 때문에 할 필요가 없다. ChangeState만 해주면 됨
}

void EventExecutor::ReadCgiResultFromPipe(KqueueHandler &kqueue_handler,
										  const int &fd, Udata *user_data) {
	char buf[ResponseMessage::BUFFER_SIZE];
	ResponseMessage &response_message = user_data->response_message_;
	ssize_t size = read(fd, buf, ResponseMessage::BUFFER_SIZE);
	if (size == 0) {
		close(fd);
		user_data->ChangeState(Udata::SEND_RESPONSE);
		kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
		return;
	}
	buf[size] = '\0';
	std::cout << buf << std::endl;	// for debugging
	if (size < 0) {
		throw HttpException(500, "read()");
	}
	response_message.AppendBody(buf);
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
void EventExecutor::ReadErrorPages(KqueueHandler &kqueue_handler, ClientSocket *client_socket, Udata *user_data) {
	ResponseMessage &response = user_data->response_message_;

	std::string error_page_path = response.GetErrorPagePath(client_socket->GetServerInfo());
	if (response.BodySize() <= 0) {
		if (error_page_path.length() > 0) {
			int error_page_fd = open(error_page_path.c_str(), O_RDONLY);
			if (error_page_fd > 0) {
				fcntl(error_page_fd, F_SETFL, O_NONBLOCK);
				kqueue_handler.DeleteWriteEvent(client_socket->GetSocketDescriptor()); // DELETE SEND_RESPONSE
				user_data->ChangeState(Udata::READ_FILE);
				kqueue_handler.AddReadEvent(error_page_fd, user_data); // ADD READ_FILE
				return;
			}
		}
		response.AppendBody(default_error_html.c_str()); // default error page
	}
}

/**
 * Response Message에 필요한 header, body가 이미 설정되었다고 가정
 * TODO: chunked response message
 */
int EventExecutor::SendResponse(KqueueHandler &kqueue_handler, ClientSocket *client_socket, Udata *user_data) {
	RequestMessage &request = user_data->request_message_;
	ResponseMessage &response = user_data->response_message_;
	int fd = client_socket->GetSocketDescriptor();

	if (response.IsErrorStatus()) {
		ReadErrorPages(kqueue_handler, client_socket, user_data);
	}
	std::string response_str = response.ToString();
	int send_len = send(fd,
						response_str.c_str() + response.current_length_,
						response_str.length() - response.current_length_, 0);
	if (send_len < 0) {
		throw HttpException(500, "send response send() error");
	}
	response.AddCurrentLength(send_len);
	if (response.IsDone()) {
		if (request.ShouldClose()) {    // connection: close
			return Udata::CLOSE;
		}
		kqueue_handler.DeleteWriteEvent(fd); // DELETE SEND_RESPONSE
		user_data->Reset();	// reset user data (state = RECV_REQUEST)
		kqueue_handler.AddReadEvent(fd, user_data);	// RECV_REQUEST
		return Udata::RECV_REQUEST;
	}
	return Udata::SEND_RESPONSE;
}
