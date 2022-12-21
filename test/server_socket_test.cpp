//
// Created by 김민준 on 2022/12/21.
//

#include "gtest/gtest.h"
#include "server_socket.hpp"
#include "core_exception.h"

TEST(server_socket_test, no_exception_test) {
	EXPECT_NO_THROW(ServerSocket server_socket("127.0.0.1", "8080"));
	EXPECT_NO_THROW(ServerSocket server_socket("127.0.0.1", "8090"));
	EXPECT_NO_THROW(ServerSocket server_socket("127.0.0.1", "8100"));
}

TEST(server_socket_test, exception_test) {
	EXPECT_NO_THROW(ServerSocket server_socket("127.0.0.1", "8080"));
	EXPECT_THROW(ServerSocket server_socket("127.0.0.1", "8080"), CoreException::BindException);
}

TEST(server_socket_test, one_success_one_fail_test) {
	EXPECT_NO_THROW(ServerSocket server_socket("0.0.0.0", "8000"));
	EXPECT_THROW(ServerSocket server_socket("0.0.0.0", "7000"), CoreException::BindException); // 이미 사용중인 포트
}