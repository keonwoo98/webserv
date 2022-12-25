//
// Created by 김민준 on 2022/12/23.
//
#include "event_handler.hpp"
#include "client_socket.hpp"
#include "request_message.hpp"
#include "response_message.hpp"
#include "request_parser.hpp"

int EventHandler::HandleListenEvent(ServerSocket server_socket) {
    int client_sock_d = server_socket.AcceptClient();
    return client_sock_d;
}

/*
 * Request Message에 resolved uri가 있는 경우
 * */


int EventHandler::HandleRequestEvent(const ClientSocket &client_socket,
                                     Udata *user_data) {
    ResponseMessage &response = user_data->response_message_;
    RequestMessage &request = user_data->request_message_;

    char tmp[BUFFER_SIZE];
    int recv_len = recv(client_socket.GetSocketDescriptor(),
                        tmp, sizeof(tmp), 0);
    if (recv_len < 0) {
        return ERROR;
    }
    tmp[recv_len] = '\0';
    ParseRequest(request, tmp);
    if (request.GetState() == DONE) {

    } else if (request.GetState() == HEADER_END) {
//        parser에서 header까지 다 읽으면 host 로 server block 선택 (client member value에 저장: ServerInfo &my_server_info_)
//        PickServerBlock(std::vector<ServerInfo> &server_infos_);

//        요청된 uri 확인해서 location block 선택 (client member value에 저장: int location_index_)
//        PickLocationBlock(std::string uri, ServerInfo &my_server_info_);

//        request 된 내용들이랑 my_server_info(server_info + location block)이 일치하는지 확인
//        RequestValidationCheck(std::string uri, ServerInfo &server_infos, int location_idx);

//        다 알맞게 들어왔다면, 들어온 uri의 실제 server상 주소를 resolve해줌
//        reslove_uri(std::string uri, ServerInfo &server_infos, int location_idx);

//        if (body_need == true
    }
//  else if (request.GetState() == Error) {

//  }
    return HAS_MORE;
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
