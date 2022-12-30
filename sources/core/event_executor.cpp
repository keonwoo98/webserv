#include <sstream>
#include <fcntl.h>

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
#include "error_pages.hpp"

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

/*
 * Request Message에 resolved uri가 있는 경우
 * TODO: kqueue_handler 사용하도록 변경
 * */
void EventExecutor::ReceiveRequest(KqueueHandler &kqueue_handler,
								   ClientSocket *client_socket,
								   const ServerSocket *server_socket,
								   Udata *user_data) {
	ResponseMessage &response = user_data->response_message_;
	(void) response;
	RequestMessage &request = user_data->request_message_;
	char buf[BUFSIZ + 1];

	int recv_len = recv(client_socket->GetSocketDescriptor(),
						buf, BUFSIZ, 0);
	if (recv_len < 0) {
		throw HttpException(INTERNAL_SERVER_ERROR, "(event_executor) : recv errror");
	}
	buf[recv_len] = '\0';
	const ConfigParser::server_infos_type &server_infos = server_socket->GetServerInfos();
	// try {
	ParseRequest(request, client_socket, server_infos, buf);
	if (request.GetState() == DONE) {
//			Resolve_URI(client_socket, request, user_data);

		std::cout << request << std::endl; // for debugging

		// make access log (request message)
		std::stringstream ss;
		ss << request << std::endl;
		kqueue_handler.AddWriteOnceEvent(Webserv::access_log_fd_, new Logger(ss.str()));
		PrepareResponse(kqueue_handler, client_socket, user_data);
	}
}

/**
 * TODO: kqueue_handler 사용 변경
 * @param fd
 * @param readable_size
 * @param response_message
 * @return
 */
void EventExecutor::ReadFile(KqueueHandler &kqueue_handler,  int fd,
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
 * Response Message에 필요한 header, body가 이미 설정되었다고 가정
 * TODO: chunked response message
 */
void EventExecutor::SendResponse(KqueueHandler &kqueue_handler, ClientSocket *client_socket, Udata *user_data) {
 	int fd = client_socket->GetSocketDescriptor();
	RequestMessage &request = user_data->request_message_;
	ResponseMessage &response = user_data->response_message_;

	if (response.IsErrorStatus()) {
		std::string error_page_path = response.GetErrorPagePath(client_socket->GetServerInfo());
		if (error_page_path.length() > 0) { // have to read error pages
			if (response.BodySize() <= 0) {
				int error_page_fd = open(error_page_path.c_str(), O_RDONLY);
				if (error_page_fd > 0) {
					kqueue_handler.DeleteWriteEvent(client_socket->GetSocketDescriptor()); // DELETE SEND_RESPONSE
					user_data->ChangeState(Udata::READ_FILE);
					kqueue_handler.AddWriteEvent(error_page_fd, user_data); // ADD READ_FILE
					return ;
				}
				response.AppendBody(ErrorPages::default_page.c_str());
			}
		} else {
			if (response.BodySize() <= 0) {
				response.AppendBody(ErrorPages::default_page.c_str());
			}
		}
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
		if (request.ShouldClose()) {	// connection: close
			delete user_data; // close socket
			user_data = NULL;
			return;
		}
		kqueue_handler.DeleteWriteEvent(fd); // DELETE SEND_RESPONSE
		user_data->Reset();	// reset user data (state = RECV_REQUEST)
		kqueue_handler.AddReadEvent(fd, user_data);	// RECV_REQUEST
	}
}

void EventExecutor::PrepareResponse(KqueueHandler &kqueue_handler,
							ClientSocket *client_socket, Udata *user_data) {
	/* test CGI */
	if (true) {
		CgiHandler cgi_handler;
		cgi_handler.SetupAndAddEvent(kqueue_handler, user_data, client_socket);
	} else {
		user_data->ChangeState(Udata::SEND_RESPONSE);
		kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
	}

	/**/
}
