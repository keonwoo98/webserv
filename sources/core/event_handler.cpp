#include "event_handler.hpp"

#include "client_socket.hpp"
#include "request_message.hpp"
#include "request_parser.hpp"
#include "request_validation.h"
#include "resolve_uri.h"
#include "response_message.hpp"

int EventHandler::HandleListenEvent(ServerSocket server_socket) {
    int client_sock_d = server_socket.AcceptClient();
    return client_sock_d;
}

/*
 * Request Message에 resolved uri가 있는 경우
 * */

int EventHandler::HandleRequestEvent(ClientSocket &client_socket,
										Udata *user_data) {
	ResponseMessage &response = user_data->response_message_;
	RequestMessage &request = user_data->request_message_;

	(void)response;
	(void)request;

	char tmp[BUFFER_SIZE];
	int recv_len = recv(client_socket.GetSocketDescriptor(),
						tmp, sizeof(tmp), 0);
	if (recv_len < 0) {
		throw (HttpException(INTERNAL_SERVER_ERROR));
	}
	tmp[recv_len] = '\0';
	try {
		// ParseRequest(request, tmp);
		const std::vector<ServerInfo> & server_infos = client_socket.GetServerInfoVector();
		ParseRequest(request, server_infos, tmp);
		if (request.GetState() == DONE) {
			std::cout << C_BOLD << C_BLUE << "PARSE DONE!" << C_RESET << std::endl;
			std::cout << request << std::endl;
			//TODO: 이 clear는 임시로 추가 한 것이다. 이후에는 response이후에 클리어 된다.
			request.Clear();
			// if (request.GetMethod() == "GET") {
			// 	return Udata::READ_FILE;
			// }
		} 
		// else if (request.GetState() == HEADER_END) { // socket info 정하고, request validation 체크하고, uri resolve
		// 	// client_socket.FindServerInfoWithHost(request.GetHeader()); // request에 GetHeader 구현 필요
		// 	client_socket.FindLocationWithUri(request.GetUri());
		// 	RequestValidationCheck(client_socket);
		// 	Resolve_URI(client_socket, request, user_data);
		// 	// exception 처리 내부에서 해줌 response 설정까지 해줘야함
		// } -> 이 부분이 ParseRequest로 들어감.
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
 * Response Message의 total_length가 Response Message를 만들 때 이미
 * 설정되었다고 가정.
 * TODO: chunked response message
 */
int EventHandler::HandleResponseEvent(const ClientSocket &client_socket,
										Udata *user_data) {
	ResponseMessage &response = user_data->response_message_;
	RequestMessage &request = user_data->request_message_;

	std::string response_str = response.ToString();
	int send_len = send(client_socket.GetSocketDescriptor(),
						response_str.c_str() + response.current_length_,
						response_str.length() - response.current_length_, 0);
	if (send_len < 0) {     // send 실패
		return ERROR;
	}
	response.AddCurrentLength(send_len);
	if (response.IsDone()) {
		RequestMessage::headers_type headers = request.GetHeaders();
		if (headers["connection"] == "close") {     // connection: close
			return CLOSE;
		}
		return KEEP_ALIVE;    // connection: keep-alive
	}
	return HAS_MORE;
}
