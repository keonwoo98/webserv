#include <sstream>

#include "event_executor.hpp"
#include "client_socket.hpp"
#include "request_message.hpp"
#include "request_parser.hpp"
#include "request_validation.hpp"
#include "response_message.hpp"
#include "http_exception.hpp"
#include "udata.hpp"
#include "webserv.hpp"
#include "logger.hpp"
#include "cgi_handler.hpp"

ClientSocket *EventExecutor::AcceptClient(KqueueHandler &kqueue_handler, ServerSocket *server_socket) {
	ClientSocket *client_socket;
	int sock_d;
	try {
		client_socket = server_socket->AcceptClient();
		sock_d = client_socket->GetSocketDescriptor();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	Udata *udata = new Udata(Udata::RECV_REQUEST, sock_d);
	kqueue_handler.AddReadEvent(sock_d, udata);

	// make access log
	std::stringstream ss;
	ss << "New Client Accepted\n" << client_socket << std::endl;

	// add events
	kqueue_handler.AddWriteOnceEvent(Webserv::access_log_fd_, new Logger(ss.str()));
	kqueue_handler.AddReadEvent(sock_d, udata); // client RECV_REQUEST
	return client_socket;
}

/*
 * Request Message에 resolved uri가 있는 경우
 * TODO: kqueue_handler 사용하도록 변경
 * */
void EventExecutor::ReceiveRequest(KqueueHandler &kqueue_handler,
								  ClientSocket *client_socket,
								  Udata *user_data) {
	(void) kqueue_handler;
	ResponseMessage &response = user_data->response_message_;
	RequestMessage &request = user_data->request_message_;

	(void) response;
	(void) request;

	char tmp[RequestMessage::BUFFER_SIZE];
	int recv_len = recv(client_socket->GetSocketDescriptor(),
						tmp, sizeof(tmp), 0);
	if (recv_len < 0) {
		throw HttpException(INTERNAL_SERVER_ERROR, "(event_executor) : recv errror");
	}
	tmp[recv_len] = '\0';
	try {
		ParseRequest(request, client_socket, tmp);
		if (request.GetState() == DONE) {
//			Resolve_URI(client_socket, request, user_data);

			// make access log (request message)
			std::stringstream ss;
			std::cout << request << std::endl; // for debugging
			ss << request << std::endl;
			kqueue_handler.AddWriteOnceEvent(Webserv::access_log_fd_, new Logger(ss.str()));

			// if (request.GetMethod() == "GET") {
			// 	return Udata::READ_FILE;
			// }
			PrepareResponse(kqueue_handler, client_socket, user_data);
		}
	} catch (const HttpException &e) {
		std::cerr << C_RED << "Exception has been thrown" << C_RESET << std::endl; // debugging
		std::cerr << C_RED << e.what() << C_RESET << std::endl; // debugging
		std::cerr << C_FAINT << request << C_RESET << std::endl;
		// response.SetStatusCode(e.GetStatusCode()); // 구현 필요
		user_data->ChangeState(Udata::SEND_RESPONSE);
		kqueue_handler.AddWriteEvent(user_data->sock_d_, user_data);
	}
}

/**
 * TODO: kqueue_handler 사용 변경
 * @param fd
 * @param readable_size
 * @param response_message
 * @return
 */
void EventExecutor::ReadFile(KqueueHandler &kqueue_handler, const int &fd,
						const int &readable_size, Udata *user_data) {
	char buf[ResponseMessage::BUFFER_SIZE];
	ResponseMessage &response_message = user_data->response_message_;
	ssize_t size = read(fd, buf, ResponseMessage::BUFFER_SIZE);
	buf[size] = '\0';
	if (size < 0) {
		throw HttpException(500, "read()");
	}
	response_message.AppendBody(buf);
	if (size < readable_size) {
		return;
	}
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
 * Response Message에 필요한 header, body가 이미 설정되었고, total_length가 계산되었다고 가정
 * TODO: chunked response message
 */
void EventExecutor::SendResponse(KqueueHandler &kqueue_handler, ClientSocket *client_socket, Udata *user_data) {
	int fd = client_socket->GetSocketDescriptor();
	ResponseMessage &response = user_data->response_message_;
	RequestMessage &request = user_data->request_message_;

	std::string response_str = response.ToString();
	int send_len = send(fd,
						response_str.c_str() + response.current_length_,
						response_str.length() - response.current_length_, 0);
	if (send_len < 0) {
		throw HttpException(500, "send response send() error");
	}
	response.AddCurrentLength(send_len);
	if (response.IsDone()) {
		RequestMessage::headers_type headers = request.GetHeaders();
		kqueue_handler.DeleteWriteEvent(fd);
		if (headers["connection"] == "close") {		// connection: close
			delete user_data;
			user_data->ChangeState(Udata::CLOSE);
			kqueue_handler.AddReadEvent(user_data->sock_d_, user_data);
		}
		user_data->Reset();	// user data reset
		user_data->ChangeState(Udata::RECV_REQUEST);
		kqueue_handler.AddReadEvent(fd, user_data);	// RECV_REQUEST
	}
}

void EventExecutor::PrepareResponse(KqueueHandler &kqueue_handler,
							ClientSocket *client_socket, Udata *user_data) {
	// if (Cgi)
	CgiHandler cgi_handler;
	cgi_handler.SetupAndAddEvent(kqueue_handler, user_data, client_socket);
}
