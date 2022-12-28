#include "event_executor.hpp"
#include "client_socket.hpp"
#include "request_message.hpp"
#include "request_parser.hpp"
#include "request_validation.hpp"
#include "response_message.hpp"
#include "http_exception.hpp"
#include "udata.hpp"

ClientSocket *EventExecutor::AcceptClient(KqueueHandler &kqueue_handler, ServerSocket *server_socket) {
	int client_sock_d = server_socket->AcceptClient();
	if (client_sock_d < 0) {
		return NULL;
	}
	ClientSocket *client_socket = new ClientSocket(client_sock_d, server_socket->GetServerInfos());
	Udata *udata = new Udata(Udata::RECV_REQUEST, client_sock_d);
	kqueue_handler.AddReadEvent(client_sock_d, udata); // client RECV_REQUEST
	return client_socket;
}

/*
 * Request Message에 resolved uri가 있는 경우
 * TODO: kqueue_handler 사용하도록 변경
 * */
int EventExecutor::ReceiveRequest(KqueueHandler &kqueue_handler,
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
		// ParseRequest(request, tmp);
		const std::vector<ServerInfo> &server_infos = client_socket->GetServerInfoVector();
		ParseRequest(request, server_infos, tmp);
		if (request.GetState() == DONE) {
			std::cout << C_BOLD << C_BLUE << "PARSE DONE!" << C_RESET << std::endl;
//			Resolve_URI(client_socket, request, user_data);
			std::cout << request << std::endl;

			//TODO: 이 clear는 임시로 추가 한 것이다. 이후에는 response이후에 클리어 된다.
			request.Clear();
			// if (request.GetMethod() == "GET") {
			// 	return Udata::READ_FILE;
			// }
		}
	} catch (const HttpException &e) {
		std::cerr << C_RED << "Exception has been thrown" << C_RESET << std::endl; // debugging
		std::cerr << C_RED << e.what() << C_RESET << std::endl; // debugging
		std::cerr << C_FAINT << request << C_RESET << std::endl;
		// response.SetStatusCode(e.GetStatusCode()); // 구현 필요
		return Udata::SEND_RESPONSE;
	}
	return Udata::RECV_REQUEST;
}

/**
 * TODO: kqueue_handler 사용 변경
 * @param fd
 * @param readable_size
 * @param response_message
 * @return
 */
int EventExecutor::ReadFile(const int &fd, const int &readable_size,
							ResponseMessage &response_message) {
	char buf[ResponseMessage::BUFFER_SIZE];
	ssize_t size = read(fd, buf, ResponseMessage::BUFFER_SIZE);
	if (size < 0) {
		throw HttpException(500, "read()");
	}
	response_message.AppendBody(buf, size);
	if (size < readable_size) {
		return Udata::READ_FILE;
	}
	return Udata::SEND_RESPONSE;
}

/**
 * Response Message에 필요한 header, body가 이미 설정되었고, total_length가 계산되었다고 가정
 * TODO: chunked response message
 */
int EventExecutor::SendResponse(KqueueHandler &kqueue_handler, ClientSocket *client_socket, Udata *user_data) {
	int fd = client_socket->GetSocketDescriptor();
	ResponseMessage &response = user_data->response_message_;
	RequestMessage &request = user_data->request_message_;

	std::string response_str = response.ToString();
	int send_len = send(fd,
						response_str.c_str() + response.current_length_,
						response_str.length() - response.current_length_, 0);
	if (send_len < 0) {
		throw HttpException(500, "send()");
	}
	response.AddCurrentLength(send_len);
	if (response.IsDone()) {
		RequestMessage::headers_type headers = request.GetHeaders();
		kqueue_handler.DeleteWriteEvent(fd);
		if (headers["connection"] == "close") {		// connection: close
			delete user_data;
			return Udata::CLOSE;
		}
		user_data->Reset();	// user data reset
		kqueue_handler.AddReadEvent(fd, user_data);	// RECV_REQUEST
		return Udata::RECV_REQUEST;		// connection: keep-alive
	}
	return Udata::SEND_RESPONSE;
}
