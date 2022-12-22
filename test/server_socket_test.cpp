//
// Created by 김민준 on 2022/12/21.
//

#include "gtest/gtest.h"
#include "server_socket.hpp"
#include "core_exception.h"

namespace {
TEST(server_socket_test, no_exception) {
	std::vector<ServerInfo> server_infos;

	std::string host_port = "127.0.0.1:8080";
	EXPECT_NO_THROW(ServerSocket server_socket(host_port, server_infos));
	host_port = "127.0.0.1:8090";
	EXPECT_NO_THROW(ServerSocket server_socket(host_port, server_infos));
	host_port = "127.0.0.1:8100";
	EXPECT_NO_THROW(ServerSocket server_socket(host_port, server_infos));
}

TEST(server_socket_test, bind_exception) {
	std::vector<ServerInfo> server_infos;

	std::string host_port = "127.0.0.1:8080";
	EXPECT_THROW(ServerSocket server_socket1(host_port, server_infos);
				ServerSocket server_socket2(host_port, server_infos), CoreException::BindException);
}

TEST(server_socket_test, one_pass_one_fail) {
	std::vector<ServerInfo> server_infos;

	std::string host_port = "0.0.0.0:8080";
	EXPECT_NO_THROW(ServerSocket server_socket(host_port, server_infos));
	host_port = "0.0.0.0:7000"; // 시스템에서 이미 사용중인 포트
	EXPECT_THROW(ServerSocket server_socket(host_port, server_infos), CoreException::BindException);
}
}
