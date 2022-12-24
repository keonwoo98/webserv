//
// Created by 김민준 on 2022/12/23.
//
#include "event_handler.hpp"
#include "client_socket.hpp"
#include "request_message.hpp"
#include "response_message.hpp"

int EventHandler::HandleListenEvent(ServerSocket server_socket) {
	int client_sock_d = server_socket.AcceptClient();
	return client_sock_d;
}

/**
 * Response Message의 total_length가 Response Message를 만들 때 이미 설정되었다고 가정.
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
	if (send_len < 0) {	 // send 실패
		return ERROR;
	}
	response.AddCurrentLength(send_len);
	if (response.IsDone()) {
		RequestMessage::headers_type headers = request.GetHeaders();
		if (headers["connection"] == "close") {	 // connection: close
			return CLOSE;
		}
		return KEEP_ALIVE;	// connection: keep-alive
	}
	return HAS_MORE;
}
