//
// Created by Alvin Lee on 2022/12/25.
//
#include "request_validation.hpp"
#include "request_message.hpp"
#include "client_socket.hpp"

// header 까지 읽은 reqeust가 유효한지 검증 with (client.server_info and client.location_index)
// 예외 발생시 execption throw 해주기
int RequestValidationCheck(const ClientSocket &client) {
	(void)client;
	return 0;
}
